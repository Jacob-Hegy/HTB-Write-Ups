void Domain_Creation(void) {
  char cVar1;
  uint _Seed;
  ulonglong absurdly_big_counter;
  undefined2 *URL_container;
  undefined2 chars_zy;
  undefined local_4a;
  undefined2 chars_x.;
  undefined local_47;
  undefined4 chars_//:;
  undefined2 chars_tt;
  undefined local_40;
  undefined8 char_set;
  undefined char_5;
  undefined local_36;
  undefined local_35;
  undefined4 the_number_27;
  undefined the_number_104;
  int random_number;
  undefined4 local_number_thing;
  undefined4 the_number_325;
  int temp;
  int counter;
  
  char_set = 0x6e7238613262636c;
  char_5 = 0x35;
  _Seed = GET_TIME((__time64_t *)0x0);
  srand(_Seed);
  for (counter = 0; counter < 6; counter = counter + 1) {
    temp = counter;
    the_number_325 = 0x145;
    local_number_thing = 0x6c;
    random_number = rand();
    counter = temp;
    random_number = random_number % 9;
    the_number_104 = 0x68;
    if (temp == 2) {
      local_35 = 4;
      local_36 = 0x62;
      uRam00000000004099a2 = 0x30;
    }
    else {
      the_number_27 = 0x1b;
      local_number_thing = 9;
      (&DAT_004099a0)[counter] = *(undefined *)((longlong)&char_set + (longlong)random_number);
    }
    local_number_thing = 3;
    the_number_104 = 0x68;
  }
  chars_tt = 0x7474;
  local_40 = 0;
  chars_//: = 0x2f2f3a;
  chars_x. = 0x782e;
  local_47 = 0;
  chars_zy = 0x7a79;
  local_4a = 0;
  URL = 0x68;
  strcat((char *)&URL,(char *)&chars_tt);
  absurdly_big_counter = 0xffffffffffffffff;
  URL_container = &URL;
  do {
    if (absurdly_big_counter == 0) break;
    absurdly_big_counter = absurdly_big_counter - 1;
    cVar1 = *(char *)URL_container;
    URL_container = (undefined2 *)((longlong)URL_container + 1);
  } while (cVar1 != '\0');
  *(undefined2 *)(~absurdly_big_counter + 0x4099ff) = 0x70;
  strcat((char *)&URL,(char *)&chars_//:);
  strcat((char *)&URL,&DAT_004099a0);
  strcat((char *)&URL,(char *)&chars_x.);
  strcat((char *)&URL,(char *)&chars_zy);
  memset(&DAT_004099a0,0,5);
  return;
}
