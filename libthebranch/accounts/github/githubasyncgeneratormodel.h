#ifndef GITHUBASYNCGENERATORMODEL_H
#define GITHUBASYNCGENERATORMODEL_H

#include "githubitem.h"
#include <QAbstractListModel>
#include <QCoroAsyncGenerator>
#include <QCoroTask>

template<IsGithubItem T>
class GitHubAsyncGeneratorModelEngine {
    public:
        explicit GitHubAsyncGeneratorModelEngine() {
            d = new GitHubAsyncGeneratorModelEnginePrivate();
        };
        ~GitHubAsyncGeneratorModelEngine() {
            delete d;
        };

    protected:
        virtual void engineCallBeginRemoveRows(QModelIndex index, int start, int count) = 0;
        virtual void engineCallEndRemoveRows() = 0;
        virtual void engineCallBeginInsertRows(QModelIndex index, int start, int count) = 0;
        virtual void engineCallEndInsertRows() = 0;

        virtual QCoro::AsyncGenerator<QSharedPointer<T>> createGenerator() = 0;

        QCoro::Task<> startFetch() {
            engineCallBeginRemoveRows(QModelIndex(), 0, this->engineRowCount());
            d->items.clear();
            engineCallEndRemoveRows();

            d->generator = createGenerator();
            d->iterator = co_await d->generator.begin();
        }

        QCoro::Task<> fetchNext() {
            if (d->fetching) co_return;

            d->fetching = true;
            if (d->requireInitialFetch) {
                d->requireInitialFetch = false;
                co_await startFetch();
            }

            for (auto i = 0; i < 100; i++) {
                if (d->iterator == d->generator.end()) {
                    d->fetching = false;
                    co_return; // Nothing else to add
                }

                engineCallBeginInsertRows(QModelIndex(), this->engineRowCount(), this->engineRowCount() + 1);
                d->items.append(*d->iterator);
                engineCallEndInsertRows();

                co_await ++(d->iterator);
            }
            d->fetching = false;
        }

        bool engineCanFetchMore() const {
            if (d->requireInitialFetch) return true;
            if (d->fetching) return true;
            return d->iterator != d->generator.end();
        }

        QSharedPointer<T> item(const QModelIndex& index) const {
            return d->items.at(index.row());
        }

        int engineRowCount() const {
            return d->items.length();
        }

    private:
        struct GitHubAsyncGeneratorModelEnginePrivate {
                QList<QSharedPointer<T>> items;
                bool fetching = false;
                bool requireInitialFetch = true;
                QCoro::AsyncGenerator<QSharedPointer<T>> generator;
                typename QCoro::AsyncGenerator<QSharedPointer<T>>::iterator iterator = typename QCoro::AsyncGenerator<QSharedPointer<T>>::iterator(nullptr);
        };

        GitHubAsyncGeneratorModelEnginePrivate* d;
};

#define GENERATOR_ENGINE                                                               \
protected:                                                                             \
    void engineCallBeginRemoveRows(QModelIndex index, int start, int count) override { \
        this->beginRemoveRows(index, start, count);                                    \
    }                                                                                  \
    void engineCallEndRemoveRows() override {                                          \
        this->endRemoveRows();                                                         \
    }                                                                                  \
    void engineCallBeginInsertRows(QModelIndex index, int start, int count) override { \
        this->beginInsertRows(index, start, count);                                    \
    }                                                                                  \
    void engineCallEndInsertRows() override {                                          \
        this->endInsertRows();                                                         \
    }                                                                                  \
    bool canFetchMore(const QModelIndex& parent) const override {                      \
        return this->engineCanFetchMore();                                             \
    }                                                                                  \
    void fetchMore(const QModelIndex& parent) override {                               \
        this->fetchNext();                                                             \
    }                                                                                  \
    int rowCount(const QModelIndex& parent) const override {                           \
        if (parent.isValid()) return 0;                                                \
        return this->engineRowCount();                                                 \
    }                                                                                  \
    bool hasChildren(const QModelIndex& parent) const override {                       \
        return false;                                                                  \
    }

#endif // GITHUBASYNCGENERATORMODEL_H
