#include "git.h"
#include <functional>
#include <git2.h>
#include <iostream>
#include <tuple>

using namespace git;

static auto lookupTree = make(git_tree_lookup, git_tree_free);
static auto openRepository = make(git_repository_open, git_repository_free);
static auto oidFromStr = make2(git_oid_fromstr);

int main() {
  git_libgit2_init();
  auto f2 = openRepository(".") //
            |
            lr::fmap([](const git::UPtr<git_repository> &repo) {
              static auto bark = TreeBark{repo};
              return bark([&repo](const TreeBark::O &o) {
                return oidFromStr("02b583822cfe94fde7ff6485dddf745dc534de22") |
                       lr::fmap([&](const auto &id) {
                         return lookupTree(repo.get(), &id);
                       }) |
                       lr::fmap([&o](const auto &pTree) {
                         const auto tree = pTree.get();
                         auto count = git_tree_entrycount(tree);
                         for (size_t i = 0; i < count; i++) {
                           const auto entry = git_tree_entry_byindex(tree, i);

                           std::cout << i << git_tree_entry_name(entry) << "\n";
                           //                               o(git_tree_entry_byindex(tree,
                           //                               i));
                         }
                         return count;
                       });
              });
            }) //
            | lr::fmap([](auto &&a) { return a; });

  std::visit(overloaded{
                 [](lr::L &&l) { std::cout << "L: " << l.message; },
                 [](git_oid &&l) { std::cout << "R: " << git_oid_tostr_s(&l); },
             },
             std::move(f2));
  std::cout << '\n';
  return 0;
}
/*
#include "lr.h"
#include <filesystem>
#include <functional>
#include <git2.h>
#include <iostream>
#include <variant>

void check_lg3(int error, const char *message, const char *extra);
auto emptyTreeSha = "4b825dc642cb6eb9a060e54bf8d69288fbee4904";

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
    using Pith = void (*)(const o, git_tree *tree, git_repository *, const bark
&);

    git_oid operator()(Pith pith) const
    {
        git_oid oid;
        git_oid_fromstr(&oid, emptyTreeSha);
        return (*this)(oid, pith);
    };

    git_oid operator()(git_oid ooid, Pith pith) const
    {
        git_treebuilder *bld = nullptr;
        git_tree *tree;
        git_tree_lookup(&tree, repo, &ooid);
        int error = git_treebuilder_new(&bld, repo, nullptr);

        pith(
            [&](entry &&r) {
                error = git_treebuilder_insert(nullptr, bld, r.name.c_str(),
&r.oid, r.type);
            },
            tree, repo, *this);

        git_oid oid;
        error = git_treebuilder_write(&oid, bld);
        git_treebuilder_free(bld);
        git_tree_free(tree);
        return oid;
    };
    bark(git_repository *repo) : repo(repo){};

private:
    git_repository *repo;
};

void p1(const bark::o o, git_tree *tree, git_repository *repo, const bark &bark)
{
    git_oid oid;
    git_blob_create_from_buffer(&oid, repo, "abo", sizeof("abo"));
    o(entry{"name1", GIT_FILEMODE_BLOB, oid});
}

void p0(const bark::o o, git_tree *tree, git_repository *repo, const bark &bark)
{
    git_oid oid;
    git_blob_create_from_buffer(&oid, repo, "abo", sizeof("abo"));
    o(entry{"name1", GIT_FILEMODE_BLOB, oid});
    o(entry{"name2", GIT_FILEMODE_TREE, bark(p1)});
}
void test();
int main()
{
    test();
    exit(9);
    git_repository *repo = nullptr;
    check_lg3(git_libgit2_init() < 0, "init", nullptr);
    //    auto cwd = std::filesystem::current_path().generic_string();
    check_lg3(git_repository_open(&repo, "."), "open", nullptr);
    git_oid oid;
    git_oid_fromstr(&oid, "d9624d8ab7559e574d00b94ceb91ffd766ee1a5f");
    auto b = bark{repo};
    oid = b(oid, p0);

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
}*/
