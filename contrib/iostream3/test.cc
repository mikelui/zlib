/*
 * Test program for gzifstream and gzofstream
 *
 * by Ludwig Schwardt <schwardt@sun.ac.za>
 * original version by Kevin Ruland <kevin@rodin.wustl.edu>
 */

#include "zfstream.h"
#include <type_traits>
#include <cassert>
#include <vector>
#include <iostream>      // for cout



void test_move() {
  static_assert(std::is_nothrow_move_constructible<gzofstream>::value
                && std::is_nothrow_move_constructible<gzifstream>::value,
                "expected to be nothrow move constructible");

  std::vector<gzofstream> outfs;
  std::vector<gzifstream> infs;

  outfs.emplace_back("test1.txt.gz");
  outfs.emplace_back();

  gzofstream& outf1 = outfs.at(0);
  gzofstream& outf2 = outfs.at(1);

  char buf[80];

  std::cout << "\n----------------\n";
  std::cout << "Same test performed while moving between two streams.\n";
  // -------------------------------------------------------------------------
  outf1 << "The quick brown fox sidestepped the lazy canine\n";

  std::cout << "\toutf1 wrote first line.\n";
  outf2 = std::move(outf1);
  outf2 << 1.3 << "\nPlan " << 9 << std::endl;
  std::cout << "\toutf2 wrote remaining lines.\n";

  outf2.close();

  std::cout << "Wrote the following message to 'test1.txt.gz' (check with zcat or zless):\n"
            << "The quick brown fox sidestepped the lazy canine\n"
            << 1.3 << "\nPlan " << 9 << std::endl;

  // -------------------------------------------------------------------------
  infs.emplace_back("test1.txt.gz");
  infs.emplace_back();

  gzifstream& inf1 = infs.at(0);
  gzifstream& inf2 = infs.at(1);

  std::cout << "\nReading 'test1.txt.gz' (buffered) produces:" << std::endl;;
  inf1.getline(buf,80,'\n');
  std::cout << "inf1 state: " << inf1.rdstate() << std::endl;
  std::cout << buf << std::endl;
  std::cout << "\tinf1:\t(" << inf1.rdbuf()->in_avail() << " chars left in buffer)\n";

  inf2 = std::move(inf1);
  std::cout << "\tmoving from inf1 to inf2" << std::endl;
  std::cout << "\tinf1:\t(" << inf1.rdbuf()->in_avail() << " chars left in buffer)\n";
  std::cout << "\tinf2:\t(" << inf2.rdbuf()->in_avail() << " chars left in buffer)\n";

  while (inf2.getline(buf,80,'\n')) {
    std::cout << buf << "\t(" << inf2.rdbuf()->in_avail() << " chars left in buffer)\n";
  }
  inf2.close();

  // -------------------------------------------------------------------------
  std::vector<gzofstream> moved_to = std::move(outfs);
  assert(outfs.empty());

  outf2.rdbuf()->pubsetbuf(0,0);
  outf2.open("test2.txt.gz");
  outf2 << setcompression(Z_NO_COMPRESSION)
       << "The quick brown fox sidestepped the lazy canine\n"
       << 1.3 << "\nPlan " << 9 << std::endl;
  std::cout << "\nWrote the same message to 'test2.txt.gz' in uncompressed form";
  outf2.close();

  std::cout << "\nReading 'test2.txt.gz' (unbuffered) produces:\n";
  inf2.rdbuf()->pubsetbuf(0,0);
  inf2.open("test2.txt.gz");
  while (inf2.getline(buf,80,'\n')) {
    std::cout << buf << "\t(" << inf2.rdbuf()->in_avail() << " chars left in buffer)\n";
  }
  inf2.close();

  assert(!inf1.eof());
  assert(inf2.eof());

  gzofstream outf3("test3.txt.gz");
  outf1 << "write to nowhere!\n";
  outf2 << "write to nowhere!\n";
  outf3 << "no problems here!\n";
  assert(outf1.bad());
  assert(outf2.bad());
  assert(outf3.good());

  outf1 = std::move(outf3);
  outf1 << "no problems here!\n";
  outf2 << "write to nowhere!\n";
  outf3 << "write to nowhere!\n";
  assert(outf1.good());
  assert(outf2.bad());
  assert(outf3.bad());
}

int main() {

  gzofstream outf;
  gzifstream inf;
  char buf[80];

  outf.open("test1.txt.gz");
  outf << "The quick brown fox sidestepped the lazy canine\n"
       << 1.3 << "\nPlan " << 9 << std::endl;
  outf.close();
  std::cout << "Wrote the following message to 'test1.txt.gz' (check with zcat or zless):\n"
            << "The quick brown fox sidestepped the lazy canine\n"
            << 1.3 << "\nPlan " << 9 << std::endl;

  std::cout << "\nReading 'test1.txt.gz' (buffered) produces:\n";
  inf.open("test1.txt.gz");
  while (inf.getline(buf,80,'\n')) {
    std::cout << buf << "\t(" << inf.rdbuf()->in_avail() << " chars left in buffer)\n";
  }
  inf.close();

  outf.rdbuf()->pubsetbuf(0,0);
  outf.open("test2.txt.gz");
  outf << setcompression(Z_NO_COMPRESSION)
       << "The quick brown fox sidestepped the lazy canine\n"
       << 1.3 << "\nPlan " << 9 << std::endl;
  outf.close();
  std::cout << "\nWrote the same message to 'test2.txt.gz' in uncompressed form";

  std::cout << "\nReading 'test2.txt.gz' (unbuffered) produces:\n";
  inf.rdbuf()->pubsetbuf(0,0);
  inf.open("test2.txt.gz");
  while (inf.getline(buf,80,'\n')) {
    std::cout << buf << "\t(" << inf.rdbuf()->in_avail() << " chars left in buffer)\n";
  }
  inf.close();

  std::cout << std::endl;
  test_move();

  return 0;

}
