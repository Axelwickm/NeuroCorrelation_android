#version 300 es

in vec4 vPosition;
in float vPotential;

uniform mat4 uMVPMatrix;

//out float opacity;

void main(){
    //Don't render if neuron position is NaN (which happens when it is deleted.)
	if (isnan(vPosition.x)){
        return;
	}

	gl_Position = uMVPMatrix * vPosition;
	gl_Position = vec4(0.5,0.2, 0, 0);

    //opacity = vPotential;
}