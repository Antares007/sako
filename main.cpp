#include <iostream>
#include <git2.h>
#include <variant>
#include <functional>
#include <filesystem>

void check_lg3(int error, const char *message, const char *extra);

using b = git_oid (*)(git_repository *);

struct entry
{
    std::string name;
    git_filemode_t type;
    git_oid oid;
};

struct bark
{
    using o = std::function<void(entry &&)>;
    using Pith = void (*)(const o, git_repository *, const bark &);

    git_oid operator()(Pith pith) const
    {
        git_treebuilder *bld = nullptr;
        int error = git_treebuilder_new(&bld, repo, nullptr);

        pith(
            [&](entry &&r) {
                error = git_treebuilder_insert(nullptr, bld, r.name.c_str(), &r.oid, r.type);
            },
            repo, *this);

        git_oid oid;
        error = git_treebuilder_write(&oid, bld);
        git_treebuilder_free(bld);
        return oid;
    };
    bark(git_repository *repo) : repo(repo){};

private:
    git_repository *repo;
};

void p1(const bark::o o, git_repository *repo, const bark &bark)
{
    git_oid oid;
    git_blob_create_from_buffer(&oid, repo, "abo", sizeof("abo"));
    o(entry{"name1", GIT_FILEMODE_BLOB, oid});
}
void p0(const bark::o o, git_repository *repo, const bark &bark)
{
    git_oid oid;
    git_blob_create_from_buffer(&oid, repo, "abo", sizeof("abo"));
    o(entry{"name1", GIT_FILEMODE_BLOB, oid});
    o(entry{"name2", GIT_FILEMODE_TREE, bark(p1)});
}

int main()
{
    git_repository *repo = nullptr;

    check_lg3(git_libgit2_init() < 0, "init", nullptr);
    auto cwd = std::filesystem::current_path().generic_string();
    check_lg3(git_repository_open(&repo, (std::filesystem::current_path().generic_string() + "/../.git").data()), "open", nullptr);

    auto oid = bark{repo}(p0);

    const auto sha1 = git_oid_tostr_s(&oid);
    std::cout << sha1 << '\n';
    git_repository_free(repo);
}

void check_lg3(int error, const char *message, const char *extra)
{
    const git_error *lg2err;
    const char *lg2msg = "", *lg2spacer = "";

    if (!error)
        return;

    if ((lg2err = git_error_last()) != NULL && lg2err->message != NULL)
    {
        lg2msg = lg2err->message;
        lg2spacer = " - ";
    }

    if (extra)
        fprintf(stderr, "%s '%s' [%d]%s%s\n",
                message, extra, error, lg2spacer, lg2msg);
    else
        fprintf(stderr, "%s [%d]%s%s\n",
                message, error, lg2spacer, lg2msg);

    exit(1);
}