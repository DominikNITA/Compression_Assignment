#ifndef CHECK_HPP
#define CHECK_HPP

#include <cassert>
#include <cstddef>
#include <vector>
#include <string>

static std::size_t tests  = 27;
static std::size_t success = 0;

void display_results()
{
  std::cout << success << "/" << tests << " tests passes avec success\n";
}

void report_error(std::string msg)
{
  std::cout << "[[ERREUR]] - " << msg << "\n";
}

void test_q1()
{
  std::vector<std::string> words = load();
  if(words.size()  == 6240   ) success++; else report_error("erreur Q1-1");
  if(words.back()  == "door" ) success++; else report_error("erreur Q1-2");
  if(words.front() == "it"   ) success++; else report_error("erreur Q1-3");
}

void test_q2()
{
  std::vector<std::string> words = load();
  occs occ = count(words);

  if(occ.size()       == 1735) success++; else report_error("erreur Q2-1");
  if(occ["the"]       ==  439) success++; else report_error("erreur Q2-2");
  if(occ["brother"]   ==   20) success++; else report_error("erreur Q2-3");
  if(occ["yellowed"]  ==    1) success++; else report_error("erreur Q2-4");
}

void test_q3()
{
  auto w = std::make_pair(std::string("foo"),std::string("01"));
  auto ww = prefix(w, "zzzz");

  if(ww.first  == "foo"   ) success++; else report_error("erreur Q3-1");
  if(ww.second == "zzzz01") success++; else report_error("erreur Q3-2");

  codes tx = {{"a","0"},{"the","1"},{"blurb","01"}};
  codes ty = {{"full","10"},{"bar","11"}};

  auto res = merge(tx,ty);

  if(res.size()  == 5 ) success++; else report_error("erreur Q3-3");
  if(res["a"]  == "00" ) success++; else report_error("erreur Q3-4");
  if(res["bar"]  == "111" ) success++; else report_error("erreur Q3-5");
  if(res["blurb"]  == "001" ) success++; else report_error("erreur Q3-6");
  if(res["full"]  == "110" ) success++; else report_error("erreur Q3-7");
  if(res["the"]  == "01" ) success++; else report_error("erreur Q3-8");

  partial_codes pc = {{12,tx}, {9,ty},{9,res}};
  auto sz = pc.size();

  auto r = extract(pc);
  if(pc.size()        == 2 ) success++; else report_error("erreur Q3-9");
  if(r.first          == 9 ) success++; else report_error("erreur Q3-10");
  if(r.second.size()  == 2 ) success++; else report_error("erreur Q3-11");

  auto rr = extract(pc);
  if(pc.size()        == 1 ) success++; else report_error("erreur Q3-12");
  if(rr.first         == 9 ) success++; else report_error("erreur Q3-13");
  if(rr.second.size() == 5 ) success++; else report_error("erreur Q3-14");

  auto rrr = extract(pc);
  if(pc.size()         == 0  ) success++; else report_error("erreur Q3-15");
  if(rrr.first         == 12 ) success++; else report_error("erreur Q3-16");
  if(rrr.second.size() == 3  ) success++; else report_error("erreur Q3-17");

  partial_codes pr = {{12,tx}, {9,ty},{9,res}};
  reduce(pr);
  if(pr.size()        == 2 ) success++; else report_error("erreur Q3-18");

  reduce(pr);
  if(pr.size()         == 1  ) success++; else report_error("erreur Q3-19");
  if(pr.begin()->first == 30 ) success++; else report_error("erreur Q3-20");
}

void run_tests()
{
  test_q1();
  test_q2();
  test_q3();
  display_results();
}

#endif
