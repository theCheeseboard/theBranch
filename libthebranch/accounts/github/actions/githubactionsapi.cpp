#include "githubactionsapi.h"

#include "../githubaccount.h"
#include "../githubhttp.h"
#include "../githubitemdatabase.h"
#include "githubworkflow.h"
#include "githubworkflowjob.h"
#include "githubworkflowrun.h"
#include "objects/remote.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

GitHubActionsApi::GitHubActionsApi(GitHubHttp* http) {
    this->http = http;
}

QCoro::AsyncGenerator<GitHubWorkflowPtr> GitHubActionsApi::listWorkflows(RemotePtr remote) {
    auto slug = remote->slugForAccount(http->account());

    for (auto page = 1;; page++) {
        auto query = QUrlQuery({
            {"page",     QString::number(page)},
            {"per_page", "100"                }
        });

        auto reply = co_await http->get(http->makeUrl(QStringLiteral("/repos/%1/actions/workflows").arg(slug), query));

        if (reply.statusCode == 200) {
            auto root = QJsonDocument::fromJson(reply.body).object();
            auto workflows = root.value("workflows").toArray();
            if (workflows.isEmpty()) co_return;

            for (auto workflow : workflows) {
                auto workflowObj = workflow.toObject();
                auto ghWorkflow = http->account()->itemDb()->update<GitHubWorkflow>(http->account(), remote, workflowObj);
                co_yield ghWorkflow;
            }
        } else {
            QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
            throw GitHubException(obj, tr("Could not get workflows"));
        }
    }
}

QCoro::Task<GitHubWorkflowPtr> GitHubActionsApi::workflow(RemotePtr remote, qint64 workflowId) {
    auto slug = remote->slugForAccount(http->account());
    auto reply = co_await http->get(QStringLiteral("/repos/%1/actions/workflows/%2").arg(slug).arg(workflowId));

    if (reply.statusCode == 200) {
        auto workflow = QJsonDocument::fromJson(reply.body).object();
        co_return http->account()->itemDb()->update<GitHubWorkflow>(http->account(), remote, workflow);
    } else {
        QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
        throw GitHubException(obj, tr("Could not get workflow"));
    }
}

QCoro::AsyncGenerator<GitHubWorkflowRunPtr> GitHubActionsApi::listWorkflowRuns(RemotePtr remote, qint64 workflowId) {
    auto slug = remote->slugForAccount(http->account());

    for (auto page = 1;; page++) {
        auto query = QUrlQuery({
            {"page",     QString::number(page)},
            {"per_page", "100"                }
        });

        auto reply = co_await http->get(http->makeUrl(QStringLiteral("/repos/%1/actions/workflows/%2/runs").arg(slug).arg(workflowId), query));

        if (reply.statusCode == 200) {
            auto root = QJsonDocument::fromJson(reply.body).object();
            auto workflows = root.value("workflow_runs").toArray();
            if (workflows.isEmpty()) co_return;

            for (auto workflow : workflows) {
                auto workflowObj = workflow.toObject();
                auto ghWorkflow = http->account()->itemDb()->update<GitHubWorkflowRun>(http->account(), remote, workflowObj);
                co_yield ghWorkflow;
            }
        } else {
            QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
            throw GitHubException(obj, tr("Could not get workflow runs"));
        }
    }
}

QCoro::Task<GitHubWorkflowRunPtr> GitHubActionsApi::workflowRun(RemotePtr remote, qint64 workflowRunId) {
    auto slug = remote->slugForAccount(http->account());
    auto reply = co_await http->get(QStringLiteral("/repos/%1/actions/runs/%2").arg(slug).arg(workflowRunId));

    if (reply.statusCode == 200) {
        auto workflowRun = QJsonDocument::fromJson(reply.body).object();
        co_return http->account()->itemDb()->update<GitHubWorkflowRun>(http->account(), remote, workflowRun);
    } else {
        QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
        throw GitHubException(obj, tr("Could not get workflow run"));
    }
}

QCoro::AsyncGenerator<GitHubWorkflowJobPtr> GitHubActionsApi::listWorkflowJobs(RemotePtr remote, qint64 workflowRunId) {
    auto slug = remote->slugForAccount(http->account());

    for (auto page = 1;; page++) {
        auto query = QUrlQuery({
            {"filter",   "latest"             },
            {"page",     QString::number(page)},
            {"per_page", "100"                }
        });

        auto reply = co_await http->get(http->makeUrl(QStringLiteral("/repos/%1/actions/runs/%2/jobs").arg(slug).arg(workflowRunId), query));

        if (reply.statusCode == 200) {
            auto root = QJsonDocument::fromJson(reply.body).object();
            auto jobs = root.value("jobs").toArray();
            if (jobs.isEmpty()) co_return;

            for (auto job : jobs) {
                auto jobObj = job.toObject();
                auto ghJob = http->account()->itemDb()->update<GitHubWorkflowJob>(http->account(), remote, jobObj);
                co_yield ghJob;
            }
        } else {
            QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
            throw GitHubException(obj, tr("Could not get workflow jobs"));
        }
    }
}

QCoro::Task<GitHubWorkflowJobPtr> GitHubActionsApi::workflowJob(RemotePtr remote, qint64 workflowJobId) {
    auto slug = remote->slugForAccount(http->account());
    auto reply = co_await http->get(QStringLiteral("/repos/%1/actions/jobs/%2").arg(slug).arg(workflowJobId));

    if (reply.statusCode == 200) {
        auto workflowJob = QJsonDocument::fromJson(reply.body).object();
        co_return http->account()->itemDb()->update<GitHubWorkflowJob>(http->account(), remote, workflowJob);
    } else {
        QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
        throw GitHubException(obj, tr("Could not get workflow job"));
    }
}
