/*
 * =============================================================
 *
 *        Project:  libpix
 *
 *       Filename:  pix.cpp
 *
 *         Editor:  vim :set et ts=2 sts=2 sw=2
 *
 *         Author:  Martin Y. Yang , yang@libpix.org
 *
 *    Description:  
 *
 * =============================================================
 */
#include <libpix.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <iostream>

using std::cout;
using std::endl;
using namespace Pix;

void help()
{
  cout<<"Usage: pix -v                                          #display system info"<<endl;
  cout<<"       pix {src.jpg}                                   #display image info"<<endl;
  cout<<"       pix -o {dst.jpg} [-q 85] [-z 800x600] {src.jpg} #resize image"<<endl;
  cout<<"Options:"<<endl;
  cout<<"  -h display this page"<<endl;
  cout<<"  -v display system info"<<endl;
  cout<<"  -o set output file"<<endl;
  cout<<"  -q set output quality"<<endl;
  cout<<"  -z resize the image, "<<endl;
  cout<<"     format: 800x600: 800 cols, 600 rows;"<<endl;
  cout<<"             800, 800x, 800x0: 800 cols, keep aspect ratio;"<<endl;
  cout<<"             x600, 0x600: 600 rows, keep aspect ratio;"<<endl;
  cout<<"  -1 resize with nearest inter-polation"<<endl;
  cout<<"  -2 resize with bilinear inter-polation"<<endl;
  cout<<"  -3 resize with bicubic inter-polation"<<endl;
  cout<<"  -s silent mode, don't show progress"<<endl;

}

bool img_info(const char *path)
{
  JpgReader jpg;
  if (!jpg.open_file(path)) return false;
  Info info;
  info<<jpg;
  String str;
  str=info;
  cout<<path<<endl;
  cout<<str.c_str()<<endl;
  return true;
}

bool img_resize(const char *dst, const char *src, int cols, int rows, pix_pol pol, int q )
{
  JpgReader jpg;
  if (!jpg.open_file(src)) return false;
  JpgWriter out;
  if (!out.open_file(dst)) return false;
  out.quality(q);
  ResizeFilter resize(pol);
  resize.open(jpg);
  resize.zoom(cols, rows);
  cout<<"resize image from "<<jpg.cols()<<'x'<<jpg.rows();
  cout<<" to "<<resize.cols()<<'x'<<resize.rows()<<endl;
  clock_t t1 = clock();
  if (!out.write_image(resize)) return false;
  clock_t t2 = clock();
  double t = t2 - t1;
  t /= CLOCKS_PER_SEC;
  cout<<"Done! "<<t<<'s'<<endl;
  return true;
}



int main(int argc, char *argv[])
{

  pix_cfg_log(3);
  if (argc == 1) {
    help();
    return 0;
  }
  int opt;
  char *dst = 0, *src = 0;
  int q = 85;
  pix_pos cols = 0, rows = 0;
  pix_pol pol = PIX_POL_BILINEAR;


  while ((opt = getopt(argc, argv, "vhs123o:z:q:")) != -1) {
    switch (opt) {
      case 'v' :
        {
          pix_sys_show();
          return 0;
        }
      case 'o' :
        {
          dst = optarg;
          break;
        }
      case 's' :
        {
          pix_cfg_log(1);
          break;

        }
      case 'q' :
        {
          q = atoi(optarg);
          break;
        }
      case 'z' :
        {
          char *p = strchr(optarg, 'x');
          cols = atoi(optarg);
          if (p) rows = atoi(p + 1);
          break;
        }
      case '1' :
        {
          pol = PIX_POL_NEAREST;
          break;
        }
      case '2' :
        {
          pol = PIX_POL_BILINEAR;
          break;
        }
      case '3' :
        {
          pol = PIX_POL_BICUBIC;
          break;
        }
      default :
        {
          return -1;
        }
    }

  }
  if (optind == 1 && argc > 1) {
    if (!img_info(argv[1])) help();
  }
  else if (dst && optind < argc ) {
    src = argv[optind];
    if (!img_resize(dst, src, cols, rows, pol, q)) help();
  } else help();

  return 0;
}
