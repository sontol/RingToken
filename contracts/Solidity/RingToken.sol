library ecmult_no_gen {
  // as a method of that object.
  function ecmult(uint256[10] params ) returns (uint256 x, uint256 y, uint256 z) {}}

library Hp{
    function deterministic_hash(uint256 x1, uint256 y1) returns (uint256 x, uint256 y){
}
}
library ModuloMultiply{
    function mulmod(uint256 x,uint256 y,uint256 z) returns(uint256){
}
}
library FieldInverse{
    function fieldinverse(uint256 x)returns (uint256){
        
    }
}
library PointonCurve{
    function IsonCurve(uint256 x, uint256 y)returns (uint256){
        
    }
}
library RingLibrary
{
function createLs(uint256[] Px,uint256[] Py,uint256[] cs,uint256[] rs) returns (uint256[19]) {
    uint256[19] memory Ls;
    uint256[10] memory params; 
    uint256 mixin=Px.length;
    for (uint i=0;i<mixin;i++)
    {
        params[0]=0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798;
        params[1]=0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8;
        params[2]=1;
        params[3]=0;
        params[4]=Px[i];
        params[5]=Py[i];
        params[6]=1;
        params[7]=0;
        params[8]=rs[i];
        params[9]=cs[i];
        (Ls[3*i],Ls[3*i+1],Ls[3*i+2]) = ecmult_no_gen.ecmult(params);
    }
    return Ls;
  }
  
  function createRs(uint256 Ix,uint256 Iy,uint256[] Px,uint256[] Py,uint256[] cs,uint256[] rs) returns (uint256[12]) {
    uint256[12] memory Rs;
    uint256[10] memory params; 
    uint256 mixin=Px.length;
    for (uint i=0;i<mixin;i++)
    {
        params[0]=Ix;
        params[1]=Iy;
        params[2]=1;
        params[3]=0;
        (params[4],params[5])=Hp.deterministic_hash(Px[i],Py[i]);
        params[6]=1;
        params[7]=0;
        params[8]=cs[i];
        params[9]=rs[i];
        (Rs[3*i],Rs[3*i+1],Rs[3*i+2]) = ecmult_no_gen.ecmult(params);
    }
    return Rs;
  }


  function create_inverse(uint256 len,uint256[8] invertee) returns (uint256[8] inverted)
  {
      uint256 u;
        inverted[0]=invertee[0];
        uint8 i=0;
        while (++i < len) {
        inverted[i]=mulmod(invertee[i],inverted[i-1],0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
    }
    u=FieldInverse.fieldinverse(inverted[--i]);
    while (i > 0) {
        uint8 j = i--;
        inverted[j]=mulmod(inverted[i],u,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
        u=mulmod(invertee[j],u,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
    }
    inverted[0]=u;
  }
 
  // In this contract, we can also directly access knownValues.flags, if we want.
}
contract RingToken{
    uint256[4] test;
    mapping (uint256 => uint)pointlist;
    event Error(bytes32 function_name, address from,uint errcode);
    event Deposited(uint256 x, uint256 y,uint256 ecdhx,uint256 ecdhy);
    function Mint(uint256[2] pubkey, uint256[2] ecdhkey) returns (bool) //ecdhpubkey is rG in CryptoNote Whitepaper
    {
        //to be user friendly we will probably need to send back to user if something goes wrong, but I'm not sure if this can somehow be abused
        uint256 Hpx;
        uint256 Hpy;
        
        if (msg.value < 1 ether)
        {
            Error("Mint",msg.sender,1);
            return false;
        }
        if (PointonCurve.IsonCurve(pubkey[0],pubkey[1])==0) {
            Error("Mint",msg.sender,2);
            return false;
        }
        if (PointonCurve.IsonCurve(ecdhkey[0],ecdhkey[1])==0){ // we probably shouldn't care if ecdhkey is valid but we do need to if we can send back just to be user friendly
            Error("Mint",msg.sender,3);
            return false;
        }
        (Hpx,Hpy)=Hp.deterministic_hash(pubkey[0],pubkey[1]);
        if (PointonCurve.IsonCurve(Hpx,Hpy)==0){//remove the possibility of an attacker sending valid pubkey but invalid Hp
            Error("Mint",msg.sender,4);
            return false;
        }
        
        //Not sure if the part above is necessary the attacker practically needs to waste ether to do this 
        if (pubkey[1]&1==0)
            pointlist[pubkey[0]]|=2; // encoding: the map is from x coordinate, bit 1 means y is odd point is a pubkey
        else
            pointlist[pubkey[0]]|=1;

        Deposited(pubkey[0],pubkey[1],ecdhkey[0],ecdhkey[1]);// bit 2 y is even point is pubkey, bit 3 y is odd point is I, bit 4 y is even point is I
        return true;                    
            
        
    }
    function Mix(uint256 Ix,uint256 Iy,uint256[] Px,uint256[] Py,uint256[] cs,uint256[] rs,uint256 Pubkeydestx,uint256 Pubkeydesty,uint256 ecdhx,uint256 ecdhy) returns(bool) {
        
        //SANITIZE,SANITIZE, SANITIZE
        if ((Px.length > 4) || (Px.length==0)){
            Error("Mix",msg.sender,0);
            return false;
        }
        if ((Px.length!=Py.length) || (Px.length!=cs.length)||(Px.length!=rs.length)){
            Error("Mix",msg.sender,0);
            return false;
        }
        for (uint256 i=0;i<Px.length;i++){
        if (PointonCurve.IsonCurve(Px[i],Py[i])==0){//This was before solidity supports mulmod/addmod, leaving it there because I'm more familiar with it
            Error("Mix",msg.sender,0);
            return false;
        }
        if(Py[i]&1==0){
            if (pointlist[Px[i]]&2==0){
                Error("Mix",msg.sender,1);
                return false;
            }
        }
       else {
            if (pointlist[Px[i]]&1==0){
                Error("Mix",msg.sender,1);
                return false;
            }
        }
        }
        uint256[19] memory temp;
        (temp[0],temp[1])=Hp.deterministic_hash(Pubkeydestx,Pubkeydesty);
        if ((PointonCurve.IsonCurve(temp[0],temp[1])==0)&&(Pubkeydesty!=0)){
            Error("Mix",msg.sender,2);
            return false;
        }
        if ((PointonCurve.IsonCurve(Pubkeydestx,Pubkeydesty)==0)&&(Pubkeydesty!=0)){
            Error("Mix",msg.sender,2);
            return false;
        }
        if (PointonCurve.IsonCurve(Ix,Iy)==0){
            Error("Mix",msg.sender,2);
            return false;
        }
        temp[0]=RingVerify.Verify(Ix,Iy,Px,Py,cs,rs,Pubkeydestx,Pubkeydesty);//VER
        if (temp[0]!=0){
            Error("Mix",msg.sender,3);
            return false;
        }
        temp[0]=Ix;//ugly hack to avoid compiler stack too deep, maybe need to change to struct for input
        if (Iy&1==0)
        {
            
            if (pointlist[temp[0]]&4!=0){ //LNK aka double spend check
                Error("Mix",msg.sender,4);
            return false;
            }
            else{
                pointlist[Ix]|=4; 
                if (Pubkeydesty!=0){
                if (Pubkeydesty&1!=0){
                    pointlist[Pubkeydestx]|=1;
                    
                }
                else{
                    pointlist[Pubkeydesty]|=2;
                }
                Deposited(Pubkeydestx,Pubkeydesty,ecdhx,ecdhy);
            }
            else{
                address(Pubkeydestx).send(1 ether);
            }
            }
        }
        else{
            if (pointlist[temp[0]]&8!=0){ //LNK aka double spend check
                Error("Mix",msg.sender,4);
            return false;
            }
            else{
                pointlist[Ix]|=8;
                if (Pubkeydesty!=0){
                if (Pubkeydesty&1!=0){
                    pointlist[Pubkeydestx]|=1;
                    
                }
                else{
                    pointlist[Pubkeydesty]|=2;
                }
                Deposited(Pubkeydestx,Pubkeydesty,ecdhx,ecdhy);
                }
                else{
                    address(Pubkeydestx).send(1 ether);
                }
            }
        }
        
    }
    function getBalance(uint256 Ix, uint256 Iy) constant returns  (uint256){
    
        if (Iy&1==0){
            return (pointlist[Ix]&4);
        }
        else{
            return (pointlist[Ix]&8);
            
        }
    }
 
   
}
library SHA{
    function SHA3(uint256 mixin,uint256[19] LRBytes)returns(uint256){}
}
library RingVerify{


function Verify(uint256 Ix,uint256 Iy,uint256[] Px,uint256[] Py,uint256[] cs,uint256[] rs,uint256 Pubkeydestx,uint256 Pubkeydesty)returns(uint256){


        uint256[19] memory LRBytes;
        uint256[12] memory Rs;
        uint256[8] memory inverts;
        uint8 i;
        uint256 temp;
        if (Px.length==1){
            return Verify_1( Ix,Iy,Px[0], Py[0], cs[0], rs[0],Pubkeydestx,Pubkeydesty);
        }
        LRBytes=RingLibrary.createLs(Px,Py,cs,rs);
        Rs=RingLibrary.createRs(Ix,Iy,Px,Py,cs,rs);
        for ( i = 0;i < Px.length;i++)
       {
           inverts[i]=LRBytes[3*i+2];
           inverts[i+Px.length]=Rs[3*i+2];
       }

       LRBytes[18]=0;

        inverts=RingLibrary.create_inverse(2*Px.length,inverts);
        for ( i= 0;i< Px.length;i++)
        {
            temp = mulmod(inverts[i],inverts[i],0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
            LRBytes[2*i] = mulmod(LRBytes[3*i],temp,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
            temp = mulmod(temp,inverts[i],0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
            LRBytes[2*i+1] = mulmod(LRBytes[3*i+1],temp,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
            
        }
        for ( i= 0;i< Px.length;i++)
        {
            temp= mulmod(inverts[i+Px.length],inverts[i+Px.length],0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
            LRBytes[2*i+2*Px.length] = mulmod(Rs[3*i],temp,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
            temp = mulmod(temp,inverts[i+Px.length],0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
            LRBytes[2*i+2*Px.length+1] = mulmod(Rs[3*i+1],temp,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
        LRBytes[18]=addmod(LRBytes[18],cs[i],0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141);
            
        }
        LRBytes[4*Px.length]=Pubkeydestx;
        LRBytes[4*Px.length+1]=Pubkeydesty;
        LRBytes[0]=SHA.SHA3(Px.length,LRBytes);
        return(LRBytes[0]^LRBytes[18]);
        //return(0);

 }
 function Verify_1(uint256 Ix,uint256 Iy,uint256 Px,uint256 Py,uint256 cs,uint256 rs,uint256 Pubkeydestx,uint256 Pubkeydesty)returns(uint256){

    uint256[10] memory params;
    uint256[19] memory LRBytes;
    uint256[8] memory inverts;
    uint256 temp;
    params[0]=0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798;
        params[1]=0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8;
        params[2]=1;
        params[3]=0;
        params[4]=Px;
        params[5]=Py;
        params[6]=1;
        params[7]=0;
        params[8]=rs;
        params[9]=cs;
        (LRBytes[0],LRBytes[1],LRBytes[2]) = ecmult_no_gen.ecmult(params);
        params[0]=Ix;
        params[1]=Iy;
        params[2]=1;
        params[3]=0;
        (params[4],params[5])=Hp.deterministic_hash(Px,Py);
        params[6]=1;
        params[7]=0;
        params[8]=cs;
        params[9]=rs;
        (LRBytes[3],LRBytes[4],LRBytes[5]) = ecmult_no_gen.ecmult(params);
        inverts[0]=LRBytes[2];
        inverts[1]=LRBytes[5];
        
        inverts=RingLibrary.create_inverse(2,inverts);
        temp = mulmod(inverts[0],inverts[0],0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
        LRBytes[0] = mulmod(LRBytes[0],temp,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
        temp = mulmod(temp,inverts[0],0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
        LRBytes[1] = mulmod(LRBytes[1],temp,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
        temp = mulmod(inverts[1],inverts[1],0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
        LRBytes[2] = mulmod(LRBytes[3],temp,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
        temp = mulmod(temp,inverts[1],0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
        LRBytes[3] = mulmod(LRBytes[4],temp,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F);
        
        LRBytes[4]=Pubkeydestx;
        LRBytes[5]=Pubkeydesty;
        
        temp=SHA.SHA3(1,LRBytes);
        return(temp^cs);
}
}
