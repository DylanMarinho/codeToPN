BEGIN { FS="\t"; }
/^ / {
  if ($3==".word") {
    printf("w:");
    code = $2;
    gsub(" ","",code);
  }
  else {
    code = $2;
    gsub(" ","",code);
    if (length(code)>4) {
      printf("a:");
    }
    else {
      printf("t:");
    }
  }
  gsub(" ","",$1);
  printf("%s%s\n",$1,code);
}
