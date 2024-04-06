void * Base64_Decoding(longlong memory_addr_of_instructions, ulonglong len_of_instructions, size_t *memory_addr_of_len_of_instructions) {
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  void *pvVar6;
  int local_10;
  int local_c;
  
  if (Base64_Lookup_Table == 0)
    FUN_00401973();
  if ((len_of_instructions & 3) == 0) {
    *memory_addr_of_len_of_instructions = (len_of_instructions >> 2) * 3;
  if (*(char *)(memory_addr_of_instructions + (len_of_instructions - 1)) == '=')
	*memory_addr_of_len_of_instructions = *memory_addr_of_len_of_instructions - 1;
  if (*(char *)(memory_addr_of_instructions + (len_of_instructions - 2)) == '=')
      *memory_addr_of_len_of_instructions = *memory_addr_of_len_of_instructions - 1;
    pvVar6 = malloc(*memory_addr_of_len_of_instructions);
  if (pvVar6 == (void *)0x0)
    pvVar6 = (void *)0x0;
  else {
    local_c = 0;
    local_10 = 0;
    while (iVar1 = local_c, (ulonglong)(longlong)local_c < len_of_instructions) {
        if (*(char *)(memory_addr_of_instructions + local_c) == '=')
          iVar2 = 0;
        else
          iVar2 = (int)*(char *)(*(char *)(memory_addr_of_instructions + local_c) + Base64_Lookup_Table);
        local_c = local_c + 1;
        if (*(char *)(memory_addr_of_instructions + local_c) == '=')
          iVar3 = 0;
        else
          iVar3 = (int)*(char *)(*(char *)(memory_addr_of_instructions + local_c) + Base64_Lookup_Table);
        local_c = iVar1 + 2;
        if (*(char *)(memory_addr_of_instructions + local_c) == '=')
          iVar4 = 0;
        else
          iVar4 = (int)*(char *)(*(char *)(memory_addr_of_instructions + local_c) + Base64_Lookup_Table);
        local_c = iVar1 + 3;
        if (*(char *)(memory_addr_of_instructions + local_c) == '=')
          iVar5 = 0;
        else
          iVar5 = (int)*(char *)(*(char *)(memory_addr_of_instructions + local_c) + Base64_Lookup_Table);
        local_c = iVar1 + 4;
        iVar5 = iVar5 + iVar2 * 0x40000 + iVar3 * 0x1000 + iVar4 * 0x40;
        if ((ulonglong)(longlong)local_10 < *memory_addr_of_len_of_instructions) {
          *(char *)((longlong)pvVar6 + (longlong)local_10) = (char)((uint)iVar5 >> 0x10);
          local_10 = local_10 + 1;
        }
        if ((ulonglong)(longlong)local_10 < *memory_addr_of_len_of_instructions) {
          *(char *)((longlong)pvVar6 + (longlong)local_10) = (char)((uint)iVar5 >> 8);
          local_10 = local_10 + 1;
        }
        if ((ulonglong)(longlong)local_10 < *memory_addr_of_len_of_instructions) {
          *(char *)((longlong)pvVar6 + (longlong)local_10) = (char)iVar5;
          local_10 = local_10 + 1;
        }
      }
    }
  }
  else
    pvVar6 = (void *)0x0;
  return pvVar6;
}
