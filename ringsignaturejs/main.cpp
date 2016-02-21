#include <iostream>
#include <array>
#include <string>
#include <sstream>
#include <iomanip>
#include <boost/random/random_device.hpp>
#define USE_SCALAR_8X32
#define USE_FIELD_5X52
#define USE_NUM_GMP
#define USE_FIELD_INV_BUILTIN
#define USE_SCALAR_INV_NU
#define USE_ASM_X86_64
#include "secp256k1.h"
#include "util.h"
#include "scalar.h"
#include "scalar_8x32_impl.h"
#include "field_impl.h"
#include "field.h"
#include "group.h"
#include "group_impl.h"
#include "ecmult_impl.h"
#include "sha3.h"
#include <node.h>


using namespace std;

namespace ringsignature{

typedef union keys {
unsigned char bytes[32];
unsigned int integers[8];
}keys;
int deterministic_hash (secp256k1_ge_t *dst, secp256k1_ge_t *src);
void print32(unsigned char *x);
void create_keys (keys *key);
static void secp256k1_ecmult_no_gen( secp256k1_gej_t *r, const secp256k1_gej_t *a, const secp256k1_gej_t *g,const secp256k1_scalar_t *na, const secp256k1_scalar_t *ng);
void createLs(secp256k1_ge_t *pubkeys, keys *q, keys *w, size_t index, size_t mixin, unsigned char *bytes);
void createRs(secp256k1_ge_t *pubkeys, secp256k1_gej_t *keyimages, keys *q, keys *w, size_t index, size_t mixin, unsigned char *bytes);

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

void create_keychain(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  keys a;
  keys b;
  string tostr;
  stringstream hexstr;
  a = create_keys(&a);
  for (unsigned i=0;i<32;i++)
        hexstr <<hex <<std::setfill ('0') << std::setw(2)<<(int)a.bytes[i];
    tostr = "0x"+hexstr.str();
      args.GetReturnValue().Set(String::NewFromUtf8(isolate, tostr.c_str()));
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "create_keychain", create_keychain);
}

NODE_MODULE(ringsignature, init)


/*int main()
{

    start:
    std::array<std::string,11> params;
    std::stringstream hexstr;
    params[0]="var a = [";
    keys *key1 =(keys *) malloc (sizeof(keys));
    create_keys(key1);
    secp256k1_scalar_t k1;
    secp256k1_scalar_set_b32(&k1,key1->bytes,NULL);


    keys *key2 =(keys *) malloc (sizeof(keys));
    create_keys(key2);
    secp256k1_scalar_t k2;
    secp256k1_scalar_set_b32(&k2,key2->bytes,NULL);


    secp256k1_gej_t G;
    secp256k1_gej_set_ge(&G,&secp256k1_ge_const_g);
    printf("here\n");


    unsigned char *zero = (unsigned char *) calloc(32,1);
    secp256k1_scalar_t zero_scalar;
    secp256k1_scalar_set_b32(&zero_scalar,zero,NULL);


    secp256k1_gej_t Pj[2];
    secp256k1_ecmult_no_gen(&Pj[0],&G,&G,&zero_scalar,&k1);
    secp256k1_fe_get_b32(zero,&Pj[0].x);
    print32(zero);
    print32(key1->bytes);
    secp256k1_fe_get_b32(zero,&Pj[0].y);
    print32(zero);
    secp256k1_ecmult_no_gen(&Pj[1],&G,&G,&zero_scalar,&k2);


    secp256k1_ge_t Ps[2];
    secp256k1_ge_set_all_gej_var(2,Ps,Pj);


    keys cs[2];
    keys rs[2];

    for (size_t i=0; i< 2;i++)
    {
        create_keys(&(cs[i]));
        create_keys(&(rs[i]));
    }

    unsigned char LRbytes[256];
    createLs(Ps,rs,cs,1,2,LRbytes);


    secp256k1_gej_t keyimages;
    secp256k1_ge_t Phash;
    secp256k1_gej_t Phashj;
    int x,y;
    y=deterministic_hash(&Phash,&Ps[0]);
    x=deterministic_hash(&Phash,&Ps[1]);
    if (!(x&&y))
        goto start;

    unsigned char public_key[32];
    secp256k1_fe_get_b32(public_key,&(Ps[0].x));
    printf ("Px0 = ");
    for (unsigned i=0;i<32;i++)
        hexstr << hex <<std::setfill ('0') << std::setw(2)<<(int)public_key[i];
    params[3]= "web3.toBigNumber(\"0x"+hexstr.str()+"\"),";
    printf("%s\n",params[3].c_str());
    hexstr.str("");
    print32(public_key);



    secp256k1_fe_get_b32(public_key,&(Ps[0].y));
    printf ("Py0 = ");
    print32(public_key);
    for (unsigned i=0;i<32;i++)
        hexstr <<hex <<std::setfill ('0') << std::setw(2)<<(int)public_key[i];
    params[4]= "web3.toBigNumber(\"0x"+hexstr.str()+"\"),";
    printf("%s\n",params[4].c_str());
    hexstr.str("");
    print32(public_key);


    secp256k1_fe_get_b32(public_key,&(Ps[1].x));
    printf ("Px1 = ");
    print32(public_key);
    for (unsigned i=0;i<32;i++)
        hexstr <<hex <<std::setfill ('0') << std::setw(2)<<(int)public_key[i];
    params[5]= "web3.toBigNumber(\"0x"+hexstr.str()+"\"),";
    printf("%s\n",params[5].c_str());
    hexstr.str("");

    secp256k1_fe_get_b32(public_key,&(Ps[1].y));
    printf ("Py1 = ");
    print32(public_key);
    for (unsigned i=0;i<32;i++)
        hexstr <<hex <<std::setfill ('0') << std::setw(2)<<(int)public_key[i];
    params[6]= "web3.toBigNumber(\"0x"+hexstr.str()+"\"),";
    printf("%s\n",params[6].c_str());
    hexstr.str("");



    secp256k1_gej_set_ge(&Phashj,&Phash);
    zero[31]=7;
    secp256k1_scalar_set_b32(&k2,zero,NULL);
    secp256k1_ecmult_no_gen(&keyimages,&Phashj,&Phashj,&k2,&zero_scalar);


    secp256k1_ge_t keyimage_affine;
    secp256k1_ge_set_all_gej_var(1,&keyimage_affine,&keyimages);
    unsigned char keyimagebinary[32];
    secp256k1_fe_normalize(&(keyimage_affine.x));
    secp256k1_fe_get_b32(keyimagebinary,&(keyimage_affine.x));
    printf("Ix =");
    print32(keyimagebinary);
    for (unsigned i=0;i<32;i++)
        hexstr <<hex <<std::setfill ('0') << std::setw(2)<<(int)keyimagebinary[i];
    params[1]= "web3.toBigNumber(\"0x"+hexstr.str()+"\"),";
    printf("%s\n",params[1].c_str());
    hexstr.str("");


    secp256k1_fe_t seven;
    secp256k1_fe_set_int(&seven,7);
    secp256k1_fe_t tempf;
    secp256k1_fe_mul(&tempf,&(keyimage_affine.x),&(keyimage_affine.x));
    secp256k1_fe_mul(&tempf,&tempf,&(keyimage_affine.x));
    secp256k1_fe_add(&tempf,&seven);
    secp256k1_fe_normalize(&tempf);
    secp256k1_fe_get_b32(public_key,&tempf);
    print32(public_key);
    secp256k1_fe_mul(&tempf,&(keyimage_affine.y),&(keyimage_affine.y));
        secp256k1_fe_normalize(&tempf);
    secp256k1_fe_get_b32(public_key,&(tempf));
    print32(public_key);

    secp256k1_fe_normalize(&(keyimage_affine.y));
    secp256k1_fe_get_b32(keyimagebinary,&(keyimage_affine.y));
    printf("Iy =");
    print32(keyimagebinary);
      for (unsigned i=0;i<32;i++)
        hexstr <<hex <<std::setfill ('0') << std::setw(2)<<(int)keyimagebinary[i];
    params[2]= "web3.toBigNumber(\"0x"+hexstr.str()+"\"),";
    hexstr.str("");
    printf("%s\n",params[2].c_str());

    createRs(Ps,&keyimages,rs,cs,1,2,LRbytes);



    unsigned char c[32];
    SHA3_256(c,LRbytes,256);

    secp256k1_scalar_t c_scalar;
    secp256k1_scalar_set_b32(&c_scalar,c,NULL);
    secp256k1_scalar_t temp;

    printf("here\n");

    for (size_t i=0;i<2;i++)
    {
        if (i!=1)
        {
            secp256k1_scalar_set_b32(&temp,cs[i].bytes,NULL);
            secp256k1_scalar_negate(&temp,&temp);
            secp256k1_scalar_add(&c_scalar,&c_scalar,&temp);
        }

    }
    secp256k1_scalar_get_b32((cs[1].bytes),&c_scalar);

    printf("here\n");
    secp256k1_scalar_negate(&temp,&c_scalar);
    secp256k1_scalar_mul(&temp,&k2,&temp);
    secp256k1_scalar_set_b32(&c_scalar,(rs[1].bytes),NULL);
    secp256k1_scalar_add(&c_scalar,&c_scalar,&temp);
    secp256k1_scalar_get_b32((rs[1].bytes),&c_scalar);
    for (size_t i=0;i<2;i++)
    {
        printf ("c%d = ",i);
        print32(cs[i].bytes);
        for (unsigned j=0;j<32;j++)
            hexstr <<hex <<std::setfill ('0') << std::setw(2)<<(int)cs[i].bytes[j];
        params[i+7]= "web3.toBigNumber(\"0x"+hexstr.str()+"\"),";
        printf("%s\n",params[i+7].c_str());
        hexstr.str("");

        printf ("r%d = ",i);
        print32(rs[i].bytes);
        for (unsigned j=0;j<32;j++)
            hexstr <<hex <<std::setfill ('0') << std::setw(2)<<(int)rs[i].bytes[j];
        params[i+9]= "web3.toBigNumber(\"0x"+hexstr.str()+"\"),";
        printf("%s\n",params[9].c_str());
        hexstr.str("");

        printf("Lx%d = ",i);
        print32(&LRbytes[i*64]);
        printf("Ly%d = ",i);
        print32(&LRbytes[i*64+32]);
        printf("Rx%d = ",i);
        print32(&LRbytes[i*64+128]);
        printf("Ry%d = ",i);
        print32(&LRbytes[i*64+32+128]);
    }
    free(zero);
    free(key2);
    free(key1);
    for (unsigned i=0;i<11;i++)
        cout << params[i];
    cout <<"]";

}*/

int deterministic_hash (secp256k1_ge_t *dst, secp256k1_ge_t *src)
{
    secp256k1_fe_t xcube;
    secp256k1_fe_t xhash;
    secp256k1_fe_t seven;
    unsigned char sha3hash[32];
    unsigned char serialized_point[64];
    secp256k1_fe_normalize(&(src->x));
    secp256k1_fe_normalize(&(src->y));
    secp256k1_fe_get_b32(serialized_point,&(src->x));
    secp256k1_fe_get_b32(&(serialized_point[32]),&(src->y));
    SHA3_256(sha3hash,serialized_point,64);
    secp256k1_fe_set_b32(&(dst->x),sha3hash);
    secp256k1_fe_set_b32(&xhash,sha3hash);
    secp256k1_fe_normalize(&xhash);
    secp256k1_fe_mul(&xcube,&xhash,&xhash);
    secp256k1_fe_mul(&xcube,&xcube,&xhash);
    secp256k1_fe_set_int(&seven,7);
    secp256k1_fe_add(&xcube,&seven);
    dst->infinity=0;
    return(secp256k1_fe_sqrt_var(&(dst->y),&xcube));
}

void create_keys (keys *key)
{
    boost::random_device random;
    for (int i=0;i<8;i++)
        key->integers[i]=random();
}

void print32(unsigned char *x)
{
    for (int i=0;i<32;i++)
        printf("%.02x",x[i]);
    printf("\n");
}

void createRs(secp256k1_ge_t *pubkeys, secp256k1_gej_t *keyimages, keys *q, keys *w, size_t index, size_t mixin, unsigned char *bytes)
{
    secp256k1_gej_t *Rjacobian = (secp256k1_gej_t *) checked_malloc(sizeof(secp256k1_gej_t)*mixin);
    secp256k1_ge_t *R = (secp256k1_ge_t * ) checked_malloc(sizeof(secp256k1_ge_t)*mixin);
    unsigned char *zero = (unsigned char *) calloc(32,1);
    secp256k1_scalar_t zero_scalar;
    secp256k1_scalar_set_b32(&zero_scalar,zero,NULL);
     for (size_t i=0;i<mixin;i++)
    {
        if (i==index)
        {
            secp256k1_scalar_t ng;
            secp256k1_gej_t temp;
            secp256k1_ge_t pointhash;
            deterministic_hash(&pointhash,&(pubkeys[i]));
            secp256k1_gej_set_ge(&temp,&pointhash);
            secp256k1_scalar_set_b32(&ng,q[i].bytes,NULL);
            secp256k1_ecmult_no_gen(&(Rjacobian[i]),&temp,&temp,&zero_scalar,&ng);
        }
        else
        {
            secp256k1_scalar_t ng;
            secp256k1_scalar_t na;
            secp256k1_gej_t temp;
            secp256k1_ge_t pointhash;
            deterministic_hash(&pointhash,&(pubkeys[i]));
            secp256k1_gej_set_ge(&temp,&pointhash);
            secp256k1_scalar_set_b32(&ng,q[i].bytes,NULL);
            secp256k1_scalar_set_b32(&na,w[i].bytes,NULL);
            secp256k1_ecmult_no_gen(&(Rjacobian[i]),keyimages,&temp,&na,&ng);
        }
    }
    secp256k1_ge_set_all_gej_var(mixin,R,Rjacobian);
    for (size_t i = 0;i<mixin;i++)
    {
        secp256k1_fe_normalize(&(R[i].x));
        secp256k1_fe_normalize(&(R[i].y));
        secp256k1_fe_get_b32(&(bytes[64*i+128]),&(R[i].x));
        secp256k1_fe_get_b32(&(bytes[64*i+32+128]),&(R[i].y));
    }
    free(Rjacobian);
    free(R);
    free(zero);
}

void createLs(secp256k1_ge_t *pubkeys, keys *q, keys *w, size_t index, size_t mixin, unsigned char *bytes)
{
    secp256k1_gej_t *Ljacobian = (secp256k1_gej_t *) checked_malloc(sizeof(secp256k1_gej_t)*mixin);
    secp256k1_ge_t *L = (secp256k1_ge_t *) checked_malloc(sizeof(secp256k1_ge_t)*mixin);
    unsigned char *zero = (unsigned char *) calloc(32,1);
    secp256k1_scalar_t zero_scalar;
    secp256k1_ecmult_context_t *ctx = (secp256k1_ecmult_context_t *) calloc(1,sizeof(secp256k1_ecmult_context_t));
    secp256k1_ecmult_context_build(ctx);
    secp256k1_scalar_set_b32(&zero_scalar,zero,NULL);
    for (size_t i=0;i<mixin;i++)
    {
        if (i==index)
        {
            //Li = q*G
            secp256k1_scalar_t ng;
            secp256k1_gej_t temp;
            secp256k1_gej_set_ge(&temp,&(pubkeys[i]));
            secp256k1_scalar_set_b32(&ng,q[i].bytes,NULL);
            secp256k1_ecmult(ctx,&(Ljacobian[i]),&temp,&zero_scalar,&ng);
        }
        else
        {
            //Li=q*G+w*Pi
            secp256k1_scalar_t ng;
            secp256k1_scalar_t na;
            secp256k1_gej_t temp;
            secp256k1_gej_set_ge(&temp,&(pubkeys[i]));
            secp256k1_scalar_set_b32(&ng,q[i].bytes,NULL);
            secp256k1_scalar_set_b32(&na,w[i].bytes,NULL);
            secp256k1_ecmult(ctx,&(Ljacobian[i]),&temp,&na,&ng);
        }
    }
    secp256k1_ge_set_all_gej_var(mixin,L,Ljacobian);
    for (size_t i = 0;i<mixin;i++)
    {
        secp256k1_fe_normalize(&(L[i].x));
        secp256k1_fe_normalize(&(L[i].y));
        secp256k1_fe_get_b32(&(bytes[64*i]),&(L[i].x));
        secp256k1_fe_get_b32(&(bytes[64*i+32]),&(L[i].y));
    }
    free(Ljacobian);
    free(L);
    free(zero);
    secp256k1_ecmult_context_clear(ctx);
    free(ctx);
}

static void secp256k1_ecmult_no_gen( secp256k1_gej_t *r, const secp256k1_gej_t *a, const secp256k1_gej_t *g,const secp256k1_scalar_t *na, const secp256k1_scalar_t *ng) {

    secp256k1_gej_t tmpj;
    secp256k1_gej_t pre_a[ECMULT_TABLE_SIZE(WINDOW_A)];
    secp256k1_gej_t pre_g[ECMULT_TABLE_SIZE(WINDOW_A)];
    secp256k1_ge_t tmpa;
#ifdef USE_ENDOMORPHISM
    secp256k1_gej_t pre_a_lam[ECMULT_TABLE_SIZE(WINDOW_A)];
    secp256k1_scalar_t na_1, na_lam;
    /* Splitted G factors. */
    secp256k1_scalar_t ng_1, ng_128;
    int wnaf_na_1[130];
    int wnaf_na_lam[130];
    int bits_na_1;
    int bits_na_lam;
    int wnaf_ng_1[129];
    int bits_ng_1;
    int wnaf_ng_128[129];
    int bits_ng_128;
#else
    int wnaf_na[256];
    int bits_na;
    int wnaf_ng[257];
    int bits_ng;
#endif
    int i;
    int bits;

#ifdef USE_ENDOMORPHISM
    /* split na into na_1 and na_lam (where na = na_1 + na_lam*lambda, and na_1 and na_lam are ~128 bit) */
    secp256k1_scalar_split_lambda_var(&na_1, &na_lam, na);

    /* build wnaf representation for na_1 and na_lam. */
    bits_na_1   = secp256k1_ecmult_wnaf(wnaf_na_1,   &na_1,   WINDOW_A);
    bits_na_lam = secp256k1_ecmult_wnaf(wnaf_na_lam, &na_lam, WINDOW_A);
    VERIFY_CHECK(bits_na_1 <= 130);
    VERIFY_CHECK(bits_na_lam <= 130);
    bits = bits_na_1;
    if (bits_na_lam > bits) {
        bits = bits_na_lam;
    }
#else
    /* build wnaf representation for na. */
    bits_na     = secp256k1_ecmult_wnaf(wnaf_na,     na,      WINDOW_A);
    bits = bits_na;
#endif

    /* calculate odd multiples of a */
    secp256k1_ecmult_table_precomp_gej_var(pre_a, a, WINDOW_A);

#ifdef USE_ENDOMORPHISM
    for (i = 0; i < ECMULT_TABLE_SIZE(WINDOW_A); i++) {
        secp256k1_gej_mul_lambda(&pre_a_lam[i], &pre_a[i]);
    }

    /* split ng into ng_1 and ng_128 (where gn = gn_1 + gn_128*2^128, and gn_1 and gn_128 are ~128 bit) */
    secp256k1_scalar_split_128(&ng_1, &ng_128, ng);

    /* Build wnaf representation for ng_1 and ng_128 */
    bits_ng_1   = secp256k1_ecmult_wnaf(wnaf_ng_1,   &ng_1,   WINDOW_G);
    bits_ng_128 = secp256k1_ecmult_wnaf(wnaf_ng_128, &ng_128, WINDOW_G);
    if (bits_ng_1 > bits) {
        bits = bits_ng_1;
    }
    if (bits_ng_128 > bits) {
        bits = bits_ng_128;
    }
#else
    bits_ng     = secp256k1_ecmult_wnaf(wnaf_ng,     ng,      WINDOW_A);
#endif

    /* calculate odd multiples of a */
    secp256k1_ecmult_table_precomp_gej_var(pre_g, g, WINDOW_A);

    if (bits_ng > bits) {
        bits = bits_ng;
    }


    secp256k1_gej_set_infinity(r);

    for (i = bits-1; i >= 0; i--) {
        int n;
        secp256k1_gej_double_var(r, r);

        if (i < bits_na && (n = wnaf_na[i])) {
            ECMULT_TABLE_GET_GEJ(&tmpj, pre_a, n, WINDOW_A);
            secp256k1_gej_add_var(r, r, &tmpj);
        }
        if (i < bits_ng && (n = wnaf_ng[i])) {
            ECMULT_TABLE_GET_GEJ(&tmpj, pre_g, n, WINDOW_A);
            secp256k1_gej_add_var(r, r, &tmpj);
        }

    }
}

}  // namespace ringsignature
