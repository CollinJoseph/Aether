#pragma once
namespace Engine {
    class Engine {
    public:
        Engine();
        void Init();
        void Run();
        void Shutdown();
    private:
        struct InternalData;
        InternalData* data; // Opaque pointer
    };
}
