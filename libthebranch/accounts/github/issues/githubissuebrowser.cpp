#include "githubissuebrowser.h"
#include "ui_githubissuebrowser.h"

#include "events/githubissuecommentbubble.h"
#include "events/githubissueeventbubble.h"
#include "githubissue.h"
#include "githubissuecommentevent.h"
#include "githubissueevent.h"
#include "githubissuemodel.h"
#include <QScrollBar>
#include <QTimer>

struct GitHubIssueBrowserPrivate {
        GitHubAccount* account;
        RemotePtr remote;
        bool isPr;
        QTimer* timer;

        QList<QWidget*> timelineWidgets;
        bool addingItems = false;

        GitHubIssueModel* issues;
        GitHubIssuePtr currentIssue;

        bool readReqiured = false;
        bool generatorStarted = false;
        int issueGeneratorIndex = 0;
        QCoro::AsyncGenerator<GitHubIssueEventPtr> issueGenerator;
        QCoro::AsyncGenerator<GitHubIssueEventPtr>::iterator issueIterator = QCoro::AsyncGenerator<GitHubIssueEventPtr>::iterator(nullptr);
};

GitHubIssueBrowser::GitHubIssueBrowser(GitHubAccount* account, RemotePtr remote, bool isPr, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GitHubIssueBrowser) {
    ui->setupUi(this);

    d = new GitHubIssueBrowserPrivate();
    d->account = account;
    d->remote = remote;
    d->isPr = isPr;

    ui->titleLabel->setText(isPr ? tr("Pull Requests") : tr("Issues"));
    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->stackedWidget->setCurrentWidget(ui->browserPage, false);

    d->issues = new GitHubIssueModel(account, remote, isPr);
    ui->listView->setModel(d->issues);

    d->timer = new QTimer(this);
    d->timer->setInterval(30000);
    connect(d->timer, &QTimer::timeout, this, [this] {
        if (d->currentIssue) d->currentIssue->fetchLatest();
    });
    d->timer->start();

    connect(
        ui->leftScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [this] {
            this->addItemsIfNeeded();
        },
        Qt::QueuedConnection);
}

GitHubIssueBrowser::~GitHubIssueBrowser() {
    delete d;
    delete ui;
}

void GitHubIssueBrowser::openIssue(GitHubIssuePtr issue) {
    for (auto* widget : d->timelineWidgets) {
        ui->timelineLayout->removeWidget(widget);
        widget->deleteLater();
    }
    d->timelineWidgets.clear();

    d->currentIssue = issue;
    ui->stackedWidget->setCurrentWidget(ui->issuePage);

    auto* firstCommentBubble = new GitHubIssueCommentBubble(issue);
    ui->timelineLayout->addWidget(firstCommentBubble);
    d->timelineWidgets.append(firstCommentBubble);

    connect(issue.data(), &GitHubIssue::updated, this, [this] {
        readCurrentIssue();
    });
    readCurrentIssue();
}

void GitHubIssueBrowser::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->browserPage);

    d->currentIssue->disconnect(this);
    d->currentIssue.clear();
}

void GitHubIssueBrowser::on_listView_clicked(const QModelIndex& index) {
    auto issue = index.data(GitHubIssueModel::IssueRole).value<GitHubIssuePtr>();
    this->openIssue(issue);
}

void GitHubIssueBrowser::readCurrentIssue() {
    // Don't mutate the generator or the iterator while items are being added
    if (d->addingItems) {
        d->readReqiured = true;
        return;
    }
    ui->titleLabel_2->setText(d->currentIssue->title());

    QPalette pal = ui->openState->palette();
    switch (d->currentIssue->state()) {
        case GitHubIssue::State::Open:
            ui->openState->setText(tr("Open"));
            pal.setColor(QPalette::Window, QColor(0, 100, 0));
            pal.setColor(QPalette::WindowText, Qt::white);
            break;
        case GitHubIssue::State::Closed:
            ui->openState->setText(tr("Closed"));
            pal.setColor(QPalette::Window, QColor(200, 0, 0));
            pal.setColor(QPalette::WindowText, Qt::white);
            break;
        case GitHubIssue::State::Merged:
            ui->openState->setText(tr("Merged"));
            break;
    }
    ui->openState->setPalette(pal);

    d->issueGeneratorIndex = 0;
    d->issueGenerator = d->currentIssue->listIssueEvents();
    d->generatorStarted = false;

    this->addItemsIfNeeded();
}

QCoro::Task<> GitHubIssueBrowser::addItemsIfNeeded() {
    if (d->addingItems) co_return;

    d->addingItems = true;
    auto cleanup = qScopeGuard([this] {
        d->addingItems = false;

        // Resume reading the current issue if it was cancelled due to items being added
        if (d->readReqiured) this->readCurrentIssue();
    });

    while (ui->leftScrollArea->verticalScrollBar()->value() > ui->leftScrollArea->verticalScrollBar()->maximum() - 300) {
        if (!d->currentIssue) co_return;

        if (d->generatorStarted) {
            if (d->issueIterator == d->issueGenerator.end()) co_return; // We've reached the end
            co_await ++(d->issueIterator);
        } else {
            d->issueIterator = co_await d->issueGenerator.begin();
            d->generatorStarted = true;
        }

        if (d->issueIterator == d->issueGenerator.end()) co_return; // We've reached the end

        // Ignore any events that we've already added
        if (d->issueGeneratorIndex < d->timelineWidgets.count() - 1 /* The initial comment */) {
            d->issueGeneratorIndex++;
            continue;
        }

        auto event = *d->issueIterator;

        QWidget* bubble;
        if (auto commentEvent = event.objectCast<GitHubIssueCommentEvent>()) {
            bubble = new GitHubIssueCommentBubble(commentEvent);
        } else {
            bubble = new GitHubIssueEventBubble(event);
        }
        ui->timelineLayout->addWidget(bubble);
        d->timelineWidgets.append(bubble);

        d->issueGeneratorIndex++;
    }
}
