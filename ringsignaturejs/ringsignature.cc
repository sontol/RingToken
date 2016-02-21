#include <iostream>
#include <array>
#include <string>
#include <sstream>
#include <iomanip>
#include <boost/random/random_device.hpp>
#include <boost/algorithm/hex.hpp>
#define USE_SCALAR_8X32
#define USE_FIELD_10X26
#define USE_NUM_GMP
#define USE_FIELD_INV_BUILTIN
#define USE_SCALAR_INV_NU
//#define USE_ASM_X86_64
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



typedef union keys {
unsigned char bytes[32];
unsigned int integers[8];
}keys;
int deterministic_hash (secp256k1_ge_t *dst, secp256k1_ge_t *src);
void print32(unsigned char *x);
void create_keys (keys *key);
static void secp256k1_ecmult_no_gen( secp256k1_gej_t *r, const secp256k1_gej_t *a, const secp256k1_gej_t *g,const secp256k1_scalar_t *na, const secp256k1_scalar_t *ng);
unsigned createLs(secp256k1_ge_t *pubkeys, keys *q, keys *w, size_t index, size_t mixin, unsigned char *bytes);
unsigned createRs(secp256k1_ge_t *pubkeys, secp256k1_gej_t *keyimages, keys *q, keys *w, size_t index, size_t mixin, unsigned char *bytes);
string byte32tohex(unsigned char * byte32);

using v8::FunctionCallbackInfo;
using v8::Exception;
using v8::Isolate;
using v8::Number;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

//TODO: replace all private key sensitive operation with side-channel safe version

void create_keychain(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  keys a;
  string retval;
  secp256k1_gej_t r;
  create_keys(&a);
  secp256k1_scalar_t na, ng;
  secp256k1_ge_t temp;
  unsigned char buf[32];

  retval = "0x" + byte32tohex(a.bytes);

	secp256k1_gej_t G;
	secp256k1_gej_set_ge(&G, &secp256k1_ge_const_g);

	secp256k1_scalar_set_int(&na, 0);
	secp256k1_scalar_set_b32(&ng, a.bytes, NULL);
	secp256k1_ecmult_no_gen(&r, &G, &G, &na, &ng);

	secp256k1_ge_set_gej(&temp, &r);
	secp256k1_fe_normalize(&(temp.x));
	secp256k1_fe_get_b32(buf, &temp.x);

	retval += "0x" + byte32tohex(buf);

	secp256k1_fe_normalize(&(temp.y));
	secp256k1_fe_get_b32(buf, &temp.y);

	retval += "0x" + byte32tohex(buf);
	
	create_keys(&a);

	retval += "0x" + byte32tohex(a.bytes);

	secp256k1_scalar_set_b32(&ng, a.bytes, NULL);
	secp256k1_ecmult_no_gen(&r, &G, &G, &na, &ng);

	secp256k1_ge_set_gej(&temp, &r);
	secp256k1_fe_normalize(&(temp.x));
	secp256k1_fe_get_b32(buf, &temp.x);

	
	retval += "0x" + byte32tohex(buf);

	secp256k1_fe_normalize(&(temp.y));
	secp256k1_fe_get_b32(buf, &temp.y);


	retval += "0x" + byte32tohex(buf);


	args.GetReturnValue().Set(String::NewFromUtf8(isolate, retval.c_str()));
}

int v8paramtobyte32(const FunctionCallbackInfo<Value>& args, unsigned index, unsigned char *retval)
{
	String::Utf8Value utf8_value(args[index]);
	string hexstring(*utf8_value);
	Isolate *isolate = args.GetIsolate();
	stringstream err;
	err << "Invalid Hex Value Argument" << index;
	for (unsigned i = 0; i < 32; i++)
	{
		if (hexstring[2 * i] <= '9' && hexstring[2 * i] >= '0')
			retval[i] = (hexstring[2 * i] - '0') << 4;
		else if (hexstring[2 * i] <= 'F' && hexstring[2 * i] >= 'A')
			retval[i] = (hexstring[2 * i] - 'A' + 10) << 4;
		else if (hexstring[2 * i] <= 'f' && hexstring[2 * i] >= 'a')
			retval[i] = (hexstring[2 * i] - 'a' + 10) << 4;
		else{
			isolate->ThrowException(Exception::TypeError(				
				String::NewFromUtf8(isolate, err.str().c_str())));
			return(-1);
		}
		if (hexstring[2 * i + 1] <= '9' && hexstring[2 * i + 1] >= '0')
			retval[i] |= (hexstring[2 * i + 1] - '0');
		else if (hexstring[2 * i + 1] <= 'F' && hexstring[2 * i + 1] >= 'A')
			retval[i] |= (hexstring[2 * i + 1] - 'A' + 10);
		else if (hexstring[2 * i + 1] <= 'f' && hexstring[2 * i + 1] >= 'a')
			retval[i] |= (hexstring[2 * i + 1] - 'a' + 10);
		else{
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, err.str().c_str())));
			return(-1);
		}
	}
	return(0);
}

int v8parampubkeytobyte32(const FunctionCallbackInfo<Value>& args, unsigned index, unsigned pubkeyindex, unsigned char *retval)
{
	String::Utf8Value utf8_value(args[index]);
	string hexstring(*utf8_value);
	Isolate *isolate = args.GetIsolate();
	stringstream err;
	err << "Invalid Hex Value Argument" << index;
	for (unsigned i = 32 * pubkeyindex; i < (32 * pubkeyindex)+32; i++)
	{
		if (2 * i + 1>hexstring.length() - 1){
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Array out of bounds")));
			return(-1);
		}

		if (hexstring[2 * i] <= '9' && hexstring[2 * i] >= '0')
			retval[i - 32 * pubkeyindex] = (hexstring[2 * i] - '0') << 4;
		else if (hexstring[2 * i] <= 'F' && hexstring[2 * i] >= 'A')
			retval[i - 32 * pubkeyindex] = (hexstring[2 * i] - 'A' + 10) << 4;
		else if (hexstring[2 * i] <= 'f' && hexstring[2 * i] >= 'a')
			retval[i - 32 * pubkeyindex] = (hexstring[2 * i] - 'a' + 10) << 4;
		else{
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, err.str().c_str())));
			return(-1);
		}
		if (hexstring[2 * i + 1] <= '9' && hexstring[2 * i + 1] >= '0')
			retval[i - 32 * pubkeyindex] |= (hexstring[2 * i + 1] - '0');
		else if (hexstring[2 * i + 1] <= 'F' && hexstring[2 * i + 1] >= 'A')
			retval[i - 32 * pubkeyindex] |= (hexstring[2 * i + 1] - 'A' + 10);
		else if (hexstring[2 * i + 1] <= 'f' && hexstring[2 * i + 1] >= 'a')
			retval[i - 32 * pubkeyindex] |= (hexstring[2 * i + 1] - 'a' + 10);
		else{
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, err.str().c_str())));
			return(-1);
		}
	}
	return(0);
}

void create_pubkey(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (args.Length() < 4) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}
	if (!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString() || !args[3]->IsString()) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Wrong arguments")));
		return;
	}

	unsigned char temp[128];


	for (unsigned j = 0; j < 4; j++)
	{
		if (v8paramtobyte32(args, j, &temp[j*32]) != 0)
			return;
		else
			continue;
	}
	
		secp256k1_gej_t A;
		secp256k1_gej_t B;
		secp256k1_gej_t Pubkey_jacobian;
		secp256k1_fe_set_b32(&(A.x), &temp[0*32]);
		secp256k1_fe_set_b32(&(A.y), &temp[1*32]);
		secp256k1_fe_set_int(&(A.z), 1);
		A.infinity = 0;
		secp256k1_fe_set_b32(&(B.x), &temp[2*32]);
		secp256k1_fe_set_b32(&(B.y), &temp[3*32]);
		secp256k1_fe_set_int(&(B.z), 1);
		B.infinity = 0;

		secp256k1_ge_t Pubkey;
		secp256k1_scalar_t secret_key;

		secp256k1_scalar_t one_scalar,zero_scalar,scalar_r;
		secp256k1_scalar_set_int(&zero_scalar, 0);
		secp256k1_scalar_set_int(&one_scalar, 1);

		unsigned ret=0;
		secp256k1_ecmult_context_t *ctx = (secp256k1_ecmult_context_t *)calloc(1, sizeof(secp256k1_ecmult_context_t));
		secp256k1_ecmult_context_build(ctx);
		if (ctx == NULL)
		{
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Failed to Allocate Memory")));
			return;
		}
		unsigned char Hs_raw[64];
		do
		{
			keys r;
			create_keys(&r);
			secp256k1_gej_t Hs;
			secp256k1_ge_t Hs_affine;
			secp256k1_ge_t Hp;
			unsigned char HsrA[32];

			

			secp256k1_scalar_set_b32(&scalar_r, r.bytes,NULL);

			secp256k1_ecmult_no_gen(&Hs, &A, &A, &scalar_r, &zero_scalar);
			secp256k1_ge_set_gej(&Hs_affine, &Hs);
			
			secp256k1_fe_normalize(&Hs_affine.x);
			secp256k1_fe_normalize(&Hs_affine.y);
			secp256k1_fe_get_b32(Hs_raw, &Hs_affine.x);
			secp256k1_fe_get_b32(&Hs_raw[32], &Hs_affine.y);

			sha3_256(HsrA, 32, Hs_raw, 64);
			secp256k1_scalar_set_b32(&secret_key, HsrA, NULL);


			
			secp256k1_ecmult(ctx, &Pubkey_jacobian, &B, &one_scalar, &secret_key);
			
			secp256k1_ge_set_gej(&Pubkey, &Pubkey_jacobian);
			if (Pubkey.infinity == 1)
				continue;
			ret = deterministic_hash(&Hp, &Pubkey);
			


		} while (ret == 0);
			
		free(ctx);
		secp256k1_ge_t rG;
		secp256k1_gej_t rG_jacobian;

		secp256k1_gej_t G;
		secp256k1_gej_set_ge(&G, &secp256k1_ge_const_g);

		secp256k1_ecmult_no_gen(&rG_jacobian, &G, &G, &zero_scalar, &scalar_r);
		secp256k1_ge_set_gej(&rG, &rG_jacobian);

		string retval;

		secp256k1_fe_normalize(&Pubkey.x);
		secp256k1_fe_normalize(&Pubkey.y);
		secp256k1_fe_get_b32(&temp[0*32], &Pubkey.x);
		secp256k1_fe_get_b32(&temp[1*32], &Pubkey.y);


		secp256k1_fe_normalize(&rG.x);
		secp256k1_fe_normalize(&rG.y);
		secp256k1_fe_get_b32(&temp[2*32], &rG.x);
		secp256k1_fe_get_b32(&temp[3*32], &rG.y);

		for (unsigned i = 0; i < 4; i++)
			retval += "0x" + byte32tohex(&temp[i*32]);
		retval += byte32tohex(Hs_raw);
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, retval.c_str()));


}


string byte32tohex(unsigned char * byte32)
{
	string retval;
	stringstream hexstr;
	for (unsigned i = 0; i < 32; i++)
		hexstr << hex << std::setfill('0') << std::setw(2) << (int)byte32[i];
	retval =  hexstr.str();
	return (retval);
}

void point_on_curve(const FunctionCallbackInfo<Value>& args)
{
	secp256k1_fe_t x;
	secp256k1_fe_t y;
	secp256k1_fe_t eqfromx;
	secp256k1_fe_t eqfromy;
	secp256k1_fe_t seven;
	unsigned char temp[32];
	Isolate* isolate = args.GetIsolate();

	if (v8paramtobyte32(args, 0, temp) != 0)
		return;
	secp256k1_fe_set_b32(&x, temp);

	if (v8paramtobyte32(args, 1, temp) != 0)
		return;
	secp256k1_fe_set_b32(&y, temp);

	secp256k1_fe_mul(&eqfromx, &x, &x);
	secp256k1_fe_mul(&eqfromx, &eqfromx, &x);
	secp256k1_fe_set_int(&seven, 7);
	secp256k1_fe_add(&eqfromx, &seven);

	secp256k1_fe_mul(&eqfromy, &y, &y);

	int isEqual = secp256k1_fe_equal_var(&eqfromx, &eqfromy);

	Local<Number> num = Number::New(isolate, isEqual);

	args.GetReturnValue().Set(num);
}

void is_our_pubkey(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();

	unsigned char temp[320];

	for (unsigned i = 0; i < 10; i++)
		if (v8paramtobyte32(args, i, &temp[i*32]) != 0)
			return;

	secp256k1_scalar_t zero_scalar;
	secp256k1_scalar_set_int(&zero_scalar, 0);

	secp256k1_scalar_t a;
	secp256k1_scalar_set_b32(&a, &temp[4*32], NULL);

	secp256k1_gej_t R_jacobian;
	secp256k1_fe_set_b32(&R_jacobian.x, &temp[2*32]);
	secp256k1_fe_set_b32(&R_jacobian.y, &temp[3*32]);
	secp256k1_fe_set_int(&R_jacobian.z, 1);
	R_jacobian.infinity = 0;

	secp256k1_gej_t aR_jacobian;
	secp256k1_ecmult_no_gen(&aR_jacobian, &R_jacobian, &R_jacobian, &zero_scalar, &a);

	secp256k1_ge_t aR;
	secp256k1_ge_set_gej(&aR, &aR_jacobian);
	secp256k1_fe_normalize(&aR.x);
	secp256k1_fe_normalize(&aR.y);

	unsigned char aR_bytes[64];
	secp256k1_fe_get_b32(aR_bytes, &aR.x);
	secp256k1_fe_get_b32(&aR_bytes[32], &aR.y);

	unsigned char HsaR_bytes[32];
	sha3_256(HsaR_bytes, 32, aR_bytes, 64);

	secp256k1_scalar_t HsaR;
	secp256k1_scalar_set_b32(&HsaR, HsaR_bytes, NULL);

	secp256k1_gej_t G;
	secp256k1_gej_set_ge(&G, &secp256k1_ge_const_g);

	secp256k1_scalar_t scalar_one;
	secp256k1_scalar_set_int(&scalar_one, 1);

	secp256k1_gej_t B;
	secp256k1_fe_set_b32(&B.x, &temp[8*32]);
	secp256k1_fe_set_b32(&B.y, &temp[9*32]);
	secp256k1_fe_set_int(&B.z, 1);
	B.infinity = 0;



	secp256k1_gej_t calculated_pubkey;

	secp256k1_ecmult_no_gen(&calculated_pubkey, &G, &B, &HsaR, &scalar_one);

	secp256k1_fe_t Px;
	secp256k1_fe_set_b32(&Px, &temp[0]);

	string retval;
	secp256k1_fe_t Py;
	secp256k1_fe_set_b32(&Py, &temp[32]);

	secp256k1_ge_t calculated_pubkey_affine;
	secp256k1_ge_set_gej(&calculated_pubkey_affine, &calculated_pubkey);

	if (secp256k1_fe_equal_var(&Px, &calculated_pubkey_affine.x) && secp256k1_fe_equal_var(&Py, &calculated_pubkey_affine.y) )
	{

				secp256k1_scalar_t b;
			secp256k1_scalar_set_b32(&b, &temp[7*32],NULL);
			unsigned char privkey[32];
			secp256k1_scalar_t privkey_scalar;
			secp256k1_scalar_add(&privkey_scalar, &b, &HsaR);
			secp256k1_ecmult_no_gen(&calculated_pubkey, &G, &G, &privkey_scalar, &zero_scalar);
			secp256k1_ge_set_gej(&calculated_pubkey_affine, &calculated_pubkey);
			if (!(secp256k1_fe_equal_var(&Px, &calculated_pubkey_affine.x) && secp256k1_fe_equal_var(&Py, &calculated_pubkey_affine.y))){
				isolate->ThrowException(Exception::TypeError(
					String::NewFromUtf8(isolate, "bG is not equal to B")));
				return;
			
			}
			secp256k1_scalar_get_b32(privkey, &privkey_scalar);
			retval = "0x" + byte32tohex(privkey);

		
			
	}
	

	args.GetReturnValue().Set(String::NewFromUtf8(isolate, retval.c_str()));
}

void create_ring_signature(const FunctionCallbackInfo<Value>& args)
{
	Isolate* isolate = args.GetIsolate();
	secp256k1_scalar_t secret_key;
	unsigned char temp[32];
	if (!args[0]->IsInt32())
	{
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Mixin must be integer")));
		return;
	}

	int mixin = (int) args[0]->NumberValue();
	secp256k1_ge_t *Pubkeys = (secp256k1_ge_t *)malloc(sizeof(secp256k1_ge_t)*mixin);

	if (Pubkeys == NULL){
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Failed to Allocate Memory")));
		return;
	}

	if (v8paramtobyte32(args,1, temp) != 0)
		return;
	
	secp256k1_scalar_set_b32(&secret_key, temp, NULL);

	boost::random_device random;
	unsigned index = random()%mixin;


	secp256k1_gej_t G;
	secp256k1_gej_set_ge(&G, &secp256k1_ge_const_g);

	secp256k1_scalar_t zero_scalar;
	secp256k1_scalar_set_int(&zero_scalar, 0);

	secp256k1_gej_t own_pubkey_jacobian;

	secp256k1_ecmult_no_gen(&own_pubkey_jacobian, &G, &G, &zero_scalar, &secret_key);
	int j = 0;

	for (int i = 0; i < mixin; i++)
	{

		if (i != index)
		{
			if (v8parampubkeytobyte32(args, 2,2*j , temp) != 0)
				return;
			secp256k1_fe_set_b32(&(Pubkeys[i].x), temp);
			if (v8parampubkeytobyte32(args, 2 ,2*j+1, temp) != 0)
				return;
			secp256k1_fe_set_b32(&(Pubkeys[i].y), temp);
			Pubkeys[i].infinity = 0;
			j++;
		}
		else
		{
			secp256k1_ge_set_gej(&Pubkeys[i], &own_pubkey_jacobian);
		}
	}
	keys *cs =(keys *) malloc(sizeof(keys)*mixin);
	keys *rs = (keys *)malloc(sizeof(keys)*mixin);
	unsigned char *LRbytes = (unsigned char *)malloc(128 * mixin + 64);

	secp256k1_ge_t Hp;
	secp256k1_gej_t I;

	if (cs == NULL || rs == NULL){
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "Failed to Allocate Memory")));
		return;
	}
	unsigned x, y;
	do{
		for (size_t i = 0; i < mixin; i++)
		{
			create_keys(&(cs[i]));
			create_keys(&(rs[i]));
		}



		x = createLs(Pubkeys, rs, cs, index, mixin, LRbytes);



		if (deterministic_hash(&Hp, &Pubkeys[index]) == 0)
		{
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "invalid pubkey")));
			return;
		}

		secp256k1_gej_t Hp_jacobian;
		secp256k1_gej_set_ge(&Hp_jacobian, &Hp);

		secp256k1_ecmult_no_gen(&I, &Hp_jacobian, &Hp_jacobian, &secret_key, &zero_scalar);

		y = createRs(Pubkeys, &I, rs, cs, index, mixin, LRbytes);
	} while ((x | y));

	
	if (v8paramtobyte32(args, 3, &LRbytes[4*32*mixin]) != 0)
		return;
	if (v8paramtobyte32(args, 4, &LRbytes[4 * 32 * mixin+32]) != 0)
		return;

	unsigned char c[32];
	SHA3_256(c, LRbytes,4*32*mixin+64);

	secp256k1_scalar_t c_scalar;
	secp256k1_scalar_set_b32(&c_scalar, c, NULL);
	secp256k1_scalar_t temp_scalar;

	for (size_t i = 0; i<mixin; i++)
	{
		if (i != index)
		{
			secp256k1_scalar_set_b32(&temp_scalar, cs[i].bytes, NULL);
			secp256k1_scalar_negate(&temp_scalar, &temp_scalar);
			secp256k1_scalar_add(&c_scalar, &c_scalar, &temp_scalar);
		}

	}
	secp256k1_scalar_get_b32((cs[index].bytes), &c_scalar);

	secp256k1_scalar_negate(&temp_scalar , &c_scalar);
	secp256k1_scalar_mul(&temp_scalar, &secret_key, &temp_scalar);
	secp256k1_scalar_set_b32(&c_scalar, (rs[index].bytes), NULL);
	secp256k1_scalar_add(&c_scalar, &c_scalar, &temp_scalar);
	secp256k1_scalar_get_b32((rs[index].bytes), &c_scalar);

	secp256k1_ge_t I_affine;
	secp256k1_ge_set_gej(&I_affine, &I);

	secp256k1_fe_normalize(&(I_affine.x));
	secp256k1_fe_normalize(&(I_affine.y));

	string retval;

	secp256k1_fe_get_b32(temp, &(I_affine.x));
	retval = "0x" + byte32tohex(temp);
	secp256k1_fe_get_b32(temp, &(I_affine.y));
	retval += "0x" + byte32tohex(temp);

	for (int i = 0; i < mixin; i++)
	{
		secp256k1_fe_normalize(&(Pubkeys[i].x));
		secp256k1_fe_get_b32(temp, &(Pubkeys[i].x));
		retval += "0x" + byte32tohex(temp);
	}

	for (int i = 0; i < mixin; i++)
	{
		secp256k1_fe_normalize(&(Pubkeys[i].y));
		secp256k1_fe_get_b32(temp, &(Pubkeys[i].y));
		retval += "0x" + byte32tohex(temp);
	}


	for (int i = 0; i < mixin; i++)
	{
		retval += "0x" + byte32tohex(rs[i].bytes);
	}

	for (int i = 0; i < mixin; i++)
	{
		retval += "0x" + byte32tohex(cs[i].bytes);
	}
	free(Pubkeys);
	free(cs);
	free(rs);
	args.GetReturnValue().Set(String::NewFromUtf8(isolate, retval.c_str()));
}






void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "create_keychain", create_keychain);
  NODE_SET_METHOD(exports, "create_pubkey", create_pubkey);
  NODE_SET_METHOD(exports, "point_on_curve", point_on_curve);
  NODE_SET_METHOD(exports, "is_our_pubkey", is_our_pubkey);
  NODE_SET_METHOD(exports, "create_ring_signature", create_ring_signature);
}

NODE_MODULE(ringsignature, init)




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

unsigned createRs(secp256k1_ge_t *pubkeys, secp256k1_gej_t *keyimages, keys *q, keys *w, size_t index, size_t mixin, unsigned char *bytes)
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
			if (Rjacobian[i].infinity != 0)
				return(1);
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
			if (Rjacobian[i].infinity != 0)
				return(1);
        }
    }
    secp256k1_ge_set_all_gej_var(mixin,R,Rjacobian);
    for (size_t i = 0;i<mixin;i++)
    {
        secp256k1_fe_normalize(&(R[i].x));
        secp256k1_fe_normalize(&(R[i].y));
        secp256k1_fe_get_b32(&(bytes[64*i+64*mixin]),&(R[i].x));
		secp256k1_fe_get_b32(&(bytes[64 * i + 32 + 64 * mixin]), &(R[i].y));
    }
    free(Rjacobian);
    free(R);
    free(zero);
	return(0);
}

unsigned createLs(secp256k1_ge_t *pubkeys, keys *q, keys *w, size_t index, size_t mixin, unsigned char *bytes)
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
			secp256k1_gej_t G;
			secp256k1_gej_set_ge(&G, &secp256k1_ge_const_g);
            secp256k1_gej_set_ge(&temp,&(pubkeys[i]));
            secp256k1_scalar_set_b32(&ng,q[i].bytes,NULL);
			secp256k1_ecmult_no_gen(&(Ljacobian[i]), &G, &G, &zero_scalar, &ng);
			if (Ljacobian[i].infinity != 0)
				return(1);
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
			if (Ljacobian[i].infinity != 0)
				return(1);
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
	return(0);
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


