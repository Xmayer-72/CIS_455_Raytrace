#include <SDL2/SDL.h>
#include <stdexcept>

int main(int argc, char const *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        throw std::runtime_error(
            std::string("could not INIT SDL2: ")
            + SDL_GetError()
        );          
    }
    
    return 0;
}
