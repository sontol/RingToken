ibrary ecmult_no_gen {
  // as a method of that object.
  function ecmult(uint256[10] params ) returns (uint256 x, uint256 y, uint256 z) {
      uint256[64] memory pre_a;
      uint256[64] memory pre_b;

      
      uint256[4] memory double_a;
      uint256[4] memory double_b;
      
      int8[257] memory wnaf_a;
      int8[257] memory wnaf_b;
      
      uint256[8] memory add_params;
      
      uint16[3] memory bits;
      
      double_a[0] = params[0];
      double_a[1] = params[1];
      double_a[2]=1;
      double_a[3]=0;
      
      double_b[0] = params[4];
      double_b[1] = params[5];
      double_b[2]=1;
      double_b[3]=0;
      
      pre_a[0] = params[0];
      pre_a[1] = params[1];
      pre_a[2]=1;
      pre_a[3]=0;
      
      pre_b[0] = params[4];
      pre_b[1] = params[5];
      pre_b[2]=1;
      pre_b[3]=0;
      
      double_a=double.ec_double(double_a);
      double_b=double.ec_double(double_b);
      
      (wnaf_a,bits[0])=ecmult_wnaf(params[8]);
      (wnaf_b,bits[1])=ecmult_wnaf(params[9]);
      
      
      
      if (bits[0]>bits[1]){
          
          bits[2]=bits[0];
      }
      else{
          bits[2]=bits[1];
      }
      for (uint256 i=1;i<16;i++){
          
          add_params[0]=pre_a[4*(i-1)];
          add_params[1]=pre_a[4*(i-1)+1];
          add_params[2]=pre_a[4*(i-1)+2];
          add_params[3]=pre_a[4*(i-1)+3];
          add_params[4]=double_a[0];
          add_params[5]=double_a[1];
          add_params[6]=double_a[2];
          add_params[7]=double_a[3];
          (pre_a[4*i],pre_a[4*i+1],pre_a[4*i+2],pre_a[4*i+3])=add.ec_add(add_params);
          add_params[0]=pre_b[4*(i-1)];
          add_params[1]=pre_b[4*(i-1)+1];
          add_params[2]=pre_b[4*(i-1)+2];
          add_params[3]=pre_b[4*(i-1)+3];
          add_params[4]=double_b[0];
          add_params[5]=double_b[1];
          add_params[6]=double_b[2];
          add_params[7]=double_b[3];
          (pre_b[4*i],pre_b[4*i+1],pre_b[4*i+2],pre_b[4*i+3])=add.ec_add(add_params);
      }
      
      double_a[3]=1;
      
      for (i=bits[2]-1;i!=0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF;i--){
          double_a=double.ec_double(double_a);
          if (wnaf_a[i]!=0&&i<bits[0]){
              if (wnaf_a[i]<0){
                add_params[0]=pre_a[4*uint256(((-1-wnaf_a[i])/2))];
                add_params[1]=0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F-pre_a[4*uint256(((-1-wnaf_a[i])/2))+1];
                add_params[2]=pre_a[4*uint256(((-1-wnaf_a[i])/2))+2];
                add_params[3]=pre_a[4*uint256(((-1-wnaf_a[i])/2))+3];
              }
             else{
                 
                 add_params[0]=pre_a[4*uint256(((wnaf_a[i]-1)/2))];
                add_params[1]=pre_a[4*uint256(((wnaf_a[i]-1)/2))+1];
                add_params[2]=pre_a[4*uint256(((wnaf_a[i]-1)/2))+2];
                add_params[3]=pre_a[4*uint256(((wnaf_a[i]-1)/2))+3];
                 
             }
             add_params[4]=double_a[0];
            add_params[5]=double_a[1];
            add_params[6]=double_a[2];
            add_params[7]=double_a[3];
            (double_a[0],double_a[1],double_a[2],double_a[3])=add.ec_add(add_params);
              
          }
          if (wnaf_b[i]!=0&&i<bits[1]){
              if (wnaf_b[i]<0){
                add_params[0]=pre_b[4*uint256(((-1-wnaf_b[i])/2))];
                add_params[1]=0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F-pre_b[4*uint256(((-1-wnaf_b[i])/2))+1];
                add_params[2]=pre_b[4*uint256(((-1-wnaf_b[i])/2))+2];
                add_params[3]=pre_b[4*uint256(((-1-wnaf_b[i])/2))+3];
              }
             else{
                 
                 add_params[0]=pre_b[4*uint256(((wnaf_b[i]-1)/2))];
                add_params[1]=pre_b[4*uint256(((wnaf_b[i]-1)/2))+1];
                add_params[2]=pre_b[4*uint256(((wnaf_b[i]-1)/2))+2];
                add_params[3]=pre_b[4*uint256(((wnaf_b[i]-1)/2))+3];
                 
             }
             add_params[4]=double_a[0];
            add_params[5]=double_a[1];
            add_params[6]=double_a[2];
            add_params[7]=double_a[3];
            (double_a[0],double_a[1],double_a[2],double_a[3])=add.ec_add(add_params);
              
          }
          
      }
      
      return(double_a[0],double_a[1],double_a[2]);


  }
  function ecmult_wnaf(uint256 a) returns(int8[257] wnaf,uint16 set_bits){
      uint256 s=a;
      uint256 count=1;
      set_bits=0;
      uint256 bit=0;
      int8 sign=1;
      uint8 x=0;
      count = (s/(2**255))&1;
      
      if (count!=0){
          s=0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141-s;
          sign=-1;
      }
      while(bit < 256) {
          uint256 now;
          uint256 word;
          if((s/(2**bit))&1==0){
              bit++;
              continue;
          }
          while (set_bits < bit) {
            wnaf[set_bits++] = 0;
        }
        now = 6;
        if (bit + now > 256) {
            now = 256 - bit;
        }
        word = uint16((s/(2**bit))&((2**now)-1));
        if (word & (2 ** (6-1))!=0){
            s+=2**(bit+6);
            wnaf[set_bits++] = sign * int8(word-64);
        }
        else{
            wnaf[set_bits++] = sign * int8(word);
        }
        bit+=now;
      }
  }

}