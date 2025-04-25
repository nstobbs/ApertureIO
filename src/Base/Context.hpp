namespace ApertureIO {
    enum RendererAPI {
        eNone = 0,
        eVulkan = 1
    };

    class Context
    {
        public:
        RendererAPI setRendererAPI(RendererAPI API);
        virtual void init() = 0;
        
        private:
        bool _started;
        static RendererAPI _rendererAPI;
    };
}