#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "hexdump.h"

void usage(char *progname)
{
  fprintf(stderr, "Usage: %s [-k <key> | -K <keyfile>] [-f <infile>] [-o <outfile>]\n", progname);
}

#define BUFSIZE 131072

int main(int argc, char *argv[])
{
  int o;
  unsigned int inbytes;
  static unsigned char keybuf[BUFSIZE];
  static unsigned char inbuf[BUFSIZE];
  unsigned int keypointer=0;
  unsigned int bufpointer;
  size_t keybytes=0;
  FILE *infile=stdin;
  FILE *outfile=stdout;
  int ownInfile=0, ownOutfile=0;
  FILE *keyfile;
  while ((o=getopt(argc, argv, "k:K:f:o:"))!=-1)
    {
      switch (o)
	{
	case 'k':
	  strncpy((char*)keybuf, optarg, sizeof(keybuf));
	  keybytes=strlen((char*)keybuf);
	  break;
	case 'K':
	  keyfile = fopen(optarg, "r");
	  if (keyfile == NULL) {
	    perror(optarg);
	    return -2;
	  }
	  keybytes=fread(keybuf, 1, sizeof(keybuf), keyfile);
	  break;
	case 'f':
	  if (!(infile=fopen(optarg,"r"))) {
	    perror(optarg);
	    ownInfile = 1;
	    return -2;
	  }
	  break;
	case 'o':
	  if (!(outfile=fopen(optarg,"w"))) {
	    perror(optarg);
	    ownOutfile = 1;
	    return -2;
	  }
	  break;
	default:
	  usage(argv[0]);
	  return -1;
	  break;
	}
    }
  if (infile == NULL || outfile == NULL) {
    usage(argv[0]);
    return -1;
  }
  if (keybytes <= 0) {
    fprintf(stderr, "No key provided.\n");
    return -2;
  }
  while ((inbytes = fread(inbuf, 1, sizeof(inbuf), infile)) > 0) {
    for (bufpointer = 0; bufpointer < inbytes; bufpointer++) {
      inbuf[bufpointer] ^= keybuf[keypointer++];
      if (keypointer >= keybytes) keypointer = 0;
    }
    fwrite(inbuf, 1, inbytes, outfile);
  }
  if (ownInfile) fclose(infile);
  if (ownOutfile) fclose(outfile);
  return 0;
}
