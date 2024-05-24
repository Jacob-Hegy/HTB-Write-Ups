int __cdecl main(int _Argc,char **_Argv,char **_Env)
{
  int iVar1;
  size_t _Size;
  undefined local_148 [3];
  undefined local_145;
  undefined8 local_128;
  undefined8 local_120;
  undefined local_108 [4];
  undefined local_104;
  undefined uStack_103;
  undefined uStack_102;
  undefined uStack_101;
  undefined4 uStack_100;
  undefined8 local_fc;
  undefined local_f0 [2];
  undefined local_ee;
  char local_98 [112];
  char *local_28;
  char *local_20;
  char *local_18;
  char *local_10;
  
  __main();
  local_10 = "shutdown /s /t 5 /c ";
  memcpy(local_108,"xeff2xC|57up1dc0w0rkccre3r00xfer",0x20);
  local_104 = 0x44;
  uStack_103 = 0x44;
  uStack_101 = 0x7b;
  memset(&local_128,0,0x1e);
  local_128 = CONCAT44(uStack_100,
                       CONCAT13(uStack_101,CONCAT12(uStack_102,CONCAT11(uStack_103,local_104))));
  local_120 = local_fc;
  local_ee = 0x7d;
  memcpy(local_148,local_f0,3);
  local_145 = 0;
  sprintf(local_98,"%s \"%s%s\"",local_10,&local_128,local_148);
  local_18 = "192.168.111.128";
  local_20 = getIPAddress();
  _Size = strlen(local_18);
  iVar1 = memcmp(local_18,local_20,_Size);
  if (iVar1 != 0) {
    local_28 = "shutdown /s /t 5 /c \"Go away\"";
    system("shutdown /s /t 5 /c \"Go away\"");
                    /* WARNING: Subroutine does not return */
    exit(-1);
  }
  system(local_98);
  return 0;
}
