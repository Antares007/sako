// clang-format off
#define Var(name) struct name
#define S(name) o("H", name);
#define FN [](const auto &o)
#define DB void operator()(const auto &o) {o("h",FN{
#define DE });}
#define _B o("t",FN{ 
#define B_ });
#define OR B_ _B
#define HM(name) _B S(name) B_
struct Ea {
  DB
    S(1) _B S(2) _B S(3) B_ B_
  OR
    S(3)
  DE
};
