#include "MyFloat.h"

MyFloat::MyFloat(){
  sign = 0;
  exponent = 0;
  mantissa = 0;
}

MyFloat::MyFloat(float f){
  unpackFloat(f);
}

MyFloat::MyFloat(const MyFloat & rhs){
	sign = rhs.sign;
	exponent = rhs.exponent;
	mantissa = rhs.mantissa;
}

ostream& operator<<(std::ostream &strm, const MyFloat &f){
	//this function is complete. No need to modify it.
	strm << f.packFloat();
	return strm;
}


MyFloat MyFloat::operator+(const MyFloat& rhs) const{

    MyFloat left(*this);
    MyFloat right(rhs);
    MyFloat result;

    // if 0
    if (left.exponent == 0 && left.mantissa == 0 && right.exponent == 0 && right.mantissa == 0) {
        return 0;
    } else if (left.exponent == 0 && left.mantissa == 0) {
        return right;
    } else if (right.exponent == 0 && right.mantissa == 0) {
        return left;
    }

    // if not 0
    if (left.sign == right.sign) {  // if same sign
        result.sign = left.sign;    // result sign will be the same as inputs
        left.mantissa |= (1 << 23); // add 1 to the left of mantissa
        right.mantissa |= (1 << 23);    // add 1 to the left of mantissa
        if (left.exponent >= right.exponent) {  // if left.exponent is larger, move right.mantissa to the right
            result.exponent = left.exponent;    // set result.exponent
            int diff_expo = left.exponent - right.exponent;
            right.mantissa = (right.mantissa >> diff_expo);
            result.mantissa = (left.mantissa + right.mantissa);  // get result mantissa

        } else {    // if right.exponent is larger, move left.mantissa to the right
            result.exponent = right.exponent;    // set result.exponent
            int diff_expo = right.exponent - left.exponent;
            left.mantissa = (left.mantissa >> diff_expo);
            result.mantissa = (left.mantissa + right.mantissa);  // get result mantissa
        }
        if (((result.mantissa >> 24) & 1) == 1) {
            result.mantissa = result.mantissa >> 1;
            result.exponent += 1;
        }
        result.mantissa &= ~(1 << 23);  // remove the 1 on the very left

    }else { // left.sign != right.sign
        left.mantissa |= (1 << 23); // add 1 to the left of mantissa
        right.mantissa |= (1 << 23);    // add 1 to the left of mantissa
        // find the larger number
        if (left.exponent > right.exponent) {   // abs of left number is larger
            result.sign = left.sign;    // set result.sign
            result.exponent = left.exponent;    // set result.exponent
            int diff_expo = left.exponent - right.exponent;

            if ((right.mantissa >> (diff_expo - 1)) & 1) {
                left.mantissa -= 1;
            }   // end if

            right.mantissa = (right.mantissa >> diff_expo);
            result.mantissa = (left.mantissa - right.mantissa);  // get result mantissa

        } else if (left.exponent < right.exponent) {    // abs of right number is larger
            result.sign = right.sign;   // set result.sign
            result.exponent = right.exponent;    // set result.exponent
            int diff_expo = right.exponent - left.exponent;

            if ((left.mantissa >> (diff_expo - 1)) & 1) {
                right.mantissa -= 1;
            }   // end if

            left.mantissa = (left.mantissa >> diff_expo);
            result.mantissa = (right.mantissa - left.mantissa);  // get result mantissa

        } else {    // same exponent
            result.exponent = left.exponent + 1;    // set result.exponent
            // compare mantissa
            if (left.mantissa == right.mantissa) {  // abs left == abs right
                return 0;

            } else if (left.mantissa > right.mantissa) {    // abs of left number is larger
                result.sign = left.sign;    // set result.sign
                result.mantissa = (left.mantissa - right.mantissa);  // set result.mantissa

            } else {    // abs of right number is larger
                result.sign = right.sign;
                result.mantissa = (right.mantissa - left.mantissa);// set result.mantissa

            }
        }
        while ((result.mantissa >> 23 & 1) == 0) {
            result.mantissa = result.mantissa << 1;
            result.exponent -= 1;
        }
        result.mantissa &= ~(1 << 23);
    }
	
	return result;  // return operator+
}

MyFloat MyFloat::operator-(const MyFloat& rhs) const{
	MyFloat right(rhs);
    // change sign and turn it to operator+
    if(right.sign == 0){
        right.sign = 1;
    }else{
        right.sign = 0;
    }
	return *this+right; // return operator-
}

bool MyFloat::operator==(const float rhs) const{
	return false; //this is just a stub so your code will compile
}


void MyFloat::unpackFloat(float f) {
    //this function must be written in inline assembly
    //extracts the fields of f into sign, exponent, and mantissa


    __asm__(
        //code
        "movl %[f], %%eax;" // Move the float f into eax
        "movb $1, %%ch;"
        "movb $31, %%cl;"
        "bextr %%ecx, %%eax, %[sign];" // Extract the sign bit using BEXTR instruction


        "movb $8, %%ch;"
        "movb $23, %%cl;"
        "bextr %%ecx, %%eax, %[exponent];" // Extract the exponent bits using BEXTR instruction

        "movb $23, %%ch;"
        "movb $0, %%cl;"
        "bextr %%ecx, %%eax, %[mantissa];" // Extract the mantissa bits using BEXTR instruction

        :
        //output
    [sign] "=r"(sign), [exponent] "=r"(exponent), [mantissa] "=r"(mantissa)
    :
    //input
    [f] "m"(f)
    //clobber
    :
    "cc", "%eax", "%ecx"

    );

}//unpackFloat

float MyFloat::packFloat() const{
    //this function must be written in inline assembly
    //returns the floating point number represented by this
    float f = 0;

    __asm__(
        //code
        "movl %[sign], %%eax;" // Move the sign variable into eax register
        "shll $31, %%eax;" // Shift left the sign bit to get the sign field
        "movl %[exponent], %%ebx;" // Move the exponent variable into ebx register
        "shll $23, %%ebx;" // Shift left the exponent field to get the exponent bits
        "orl %%eax, %%ebx;" // OR the sign and exponent bits
        "movl %[mantissa], %%ecx;" // Move the mantissa variable into ecx register
        "orl %%ecx, %%ebx;" // OR the mantissa bits with the existing bits
        "movl %%ebx, %[f];" // Move the result into f variable

        //output
        :
    [f] "+m"(f)
    :
    //input
    [sign] "r"(sign), [exponent] "r"(exponent), [mantissa] "r"(mantissa)

    //clobber
    :
    "cc", "%eax", "%ebx", "%ecx"
    );

    return f;
}//packFloat
//



