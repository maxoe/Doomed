#version 400 core
                                                                                    
in vec2 texCoord;      

uniform sampler2D shadowMap;                                                       
                                                                                    
out vec4 fragColor;                                                                 
                                                                                    
void main()                                                                         
{                                                                                   
    float depth = texture(shadowMap, texCoord).x;
    depth = 1.0 - (1.0 - depth) * 25.0;
    fragColor = vec4(depth);
}
