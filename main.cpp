#include <iostream>
#include <git2.h>
#include <iostream>
#include <iomanip>
#include <OpenXLSX.h>
#include <named_type.hpp>

using namespace std;
using namespace OpenXLSX;

void check_lg3(int error, const char *message, const char *extra);
template <typename Function>
using Comparator = fluent::NamedType<Function, struct ComparatorTag>;

template <typename Function>
void performAction(Comparator<Function> comp)
{
    auto a = comp.get();
    a(1);
}

int main()
{
    auto x = fluent::make_named<Comparator>([](int x) { std::cout << "compare\n"; });
    performAction(x);

    git_repository *repo = nullptr;

    check_lg3(git_libgit2_init() < 0, "init", nullptr);
    check_lg3(git_repository_open(&repo, "."), "open", nullptr);
    git_oid oid;
    git_commit *commit = nullptr;

    const char *sha = "28db7ff9455022c7e4e70220b4bb360721d2274c";
    check_lg3(git_oid_fromstr(&oid, sha), "oid", nullptr);

    git_commit_lookup(&commit, repo, &oid);
    git_tree *tree = nullptr;
    check_lg3(git_commit_tree(&tree, commit), "get tree", nullptr);
    auto len = git_tree_entrycount(tree);

    git_tree_free(tree);
    git_commit_free(commit);
    git_repository_free(repo);

    /* XLDocument doc;
    doc.CreateDocument("./MyTest.xlsx");
    auto wks = doc.Workbook().Worksheet("Sheet1");

    wks.Cell("A1").Value() = 3.14159;
    wks.Cell("B1").Value() = 42;
    wks.Cell("C1").Value() = "Hello OpenXLSX!";
    wks.Cell("D1").Value() = true;
    wks.Cell("E1").Value() = wks.Cell("C1").Value();

    auto A1 = wks.Cell("A1").Value().Get<double>();
    auto B1 = wks.Cell("B1").Value().Get<unsigned int>();
    auto C1 = wks.Cell("C1").Value().Get<std::string>();
    auto D1 = wks.Cell("D1").Value().Get<bool>();
    auto E1 = wks.Cell("E1").Value().Get<std::string>();

    auto val = wks.Cell("E1").Value();
    cout << val.Get<std::string>() << endl;

    cout << "Cell A1: " << A1 << endl;
    cout << "Cell B1: " << B1 << endl;
    cout << "Cell C1: " << C1 << endl;
    cout << "Cell D1: " << D1 << endl;
    cout << "Cell E1: " << E1 << endl;

    doc.SaveDocument(); */

    return 0;
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