//K is the number of columns. A, B, and C are in sparse form
void mySparseMM(void* sys_args, void* app_args){
  struct myArgsType{int K;double* A_val_ptr;int* A_row_ptr;int* A_col_idx_ptr;double* B_val_ptr;int* B_row_ptr;int* B_col_idx_ptr;double* C_val_ptr;int* C_row_ptr;int* C_col_idx_ptr;};
  int* pos= (int*)sys_args;
  int blockId_x= pos[0]; int threadId_x= pos[1]; 
  int dimGrid_x= pos[2]; int dimBlock_x= pos[3];
  myArgsType *args= (myArgsType*)app_args;
  int K= args->K; double* A_val_ptr= args->A_val_ptr; //likewise for A_row_ptr, A_col_idx_ptr, B_val_ptr, B_row_ptr, B_col_idx_ptr, C_val_ptr, C_row_ptr, C_col_idx_ptr

  int teamID=threadId_x/THREADS_PER_TEAM;
  int tid= threadId_x%THREADS_PER_TEAM; 
  int nTeams= dimBlock_x/THREADS_PER_TEAM; 
  for (int k=teamID; k <K; k+= nTeams) {
    //each thread block (group of thread teams) takes a row of A
    for (int a_idx = A_col_idx_ptr[k]+blockId_x; a_idx < A_col_idx_ptr[k+1]; a_idx+=dimGrid_x) {
      int j = A_row_ptr[a_idx];
      double a_val = A_val_ptr[a_idx];
      if (a_val != 0){
        int c_idx = C_col_idx_ptr[j];
        //parallelize columns of B using threads in a team
        for (int b_idx = B_col_idx_ptr[k]+tid; b_idx < 
          B_col_idx_ptr[k+1]; b_idx+=THREADS_PER_TEAM) {
          int i = B_row_ptr[b_idx];
          int c_end= C_col_idx_ptr[j+1];
          double b_val = B_val_ptr[b_idx];
          while (c_idx<c_end && C_row_ptr[c_idx]<i) ++c_idx;
          myAtomicAdd(&C_val_ptr[c_idx], -a_val * b_val);
}}}}}

