#include <defs.h>
#include <string.h>
#include <stdio.h>
#include <task.h>
#include <virt_mem.h>
#include <sys/gdt.h>
#include <stdlib.h>
#include <virt_mem.h>
#include <idt.h>

extern uint32_t next_pid;

page_directory_t* clone_page_directory(page_directory_t* tab_src, int level) {

  page_directory_t* tab_new = (page_directory_t*)i_virt_alloc();
  int i = 0;
  for (i = 0; i < 512; i++) {
    if (is_present(&tab_src->entries[i])) {
      uint64_t virt = i_phy_to_virt(tab_src->entries[i] & MEM_FLAG_MASK);
      if (level == 1) {
          tab_new->entries[i] = tab_src->entries[i];
      } else {
        page_directory_t* newPage = clone_page_directory((page_directory_t*)virt, level - 1);
        tab_new->entries[i] = i_virt_to_phy((uint64_t)newPage) | (tab_src->entries[i] & 0XFFF);
      }
    }
  }
  return tab_new;
}

extern uint64_t _read_rip();

uint32_t kfork(struct regsForSyscall *regs) {

   task_t *parent_task = (task_t*)current_task;
   task_t *new_task = get_next_free_task();
   new_task->id = next_pid++;
   for(int i = 0; i < 3; i++)
      new_task->open_files[i] = 1;


   char **envp = (char**)current_task->envp;
   int i = 0;
   for (;  envp && envp[i]; i++) {
     if (DEBUG) printf("execve: Copying envp %d %s\n", i, envp[i]);
     new_task->envp[i] = (char*)kmalloc(strlen(envp[i]) + 1);
     strcpy(new_task->envp[i], envp[i]);
   }
   new_task->envp[i] = NULL;

   //current_task->program_name = kmalloc(64);
   strcpy(new_task->temp_buffer, parent_task->program_name);

   new_task->pml4e = clone_page_directory(current_task->pml4e, 4);
   
   for (int i = 0; i <10; i++) {
     if (current_task->vma[i].start_addr == NULL)
       continue;
     new_task->vma[i].start_addr = current_task->vma[i].start_addr;
     new_task->vma[i].end_addr = current_task->vma[i].end_addr;

     uint64_t cur_addr = new_task->vma[i].start_addr;
     for(; cur_addr < new_task->vma[i].end_addr; cur_addr += 4096) {
       uint64_t par_virt = (uint64_t)i_virt_alloc();
       map_process_specific(cur_addr, i_virt_to_phy(par_virt), new_task->pml4e, new_task);
       memcpy((void*)par_virt, (void*)cur_addr, 4096);
     }
   }
   new_task->mem_limit = current_task->mem_limit;
   new_task->rsp = (uint64_t)regs; 
   new_task->u_rsp = current_task->u_rsp;
   new_task->tss_rsp = current_task->tss_rsp;
   new_task->parent = (task_t*)current_task;
   new_task->new_proc = 1;
   new_task->STATUS = TASK_READY;
   strcpy((char*)new_task->pwd, (char*)current_task->pwd);
   strcpy(new_task->program_name, new_task->temp_buffer);
   return new_task->id;
}


