.. _cache_framework_overview:



========
Overview
========

The LVGL Cache Framework is a general-purpose, efficient caching system designed to
optimize resource usage and improve graphics rendering performance in embedded
systems. It provides a flexible architecture that can be adapted to various caching
needs.



System Architecture
*******************

The following diagram illustrates how the Cache Framework integrates with various LVGL
components and supports different resource types:

.. mermaid::
   :zoom:

   %%{init: {'theme':'neutral', 'flowchart': {'curve': 'basis'}}}%%
   graph LR
       classDef core fill:#e1f5fe,stroke:#01579b,stroke-width:2px;
       classDef cache fill:#fff3e0,stroke:#e65100,stroke-width:2px;
       classDef resource fill:#e8f5e9,stroke:#1b5e20,stroke-width:2px;

       subgraph LVGL_Core [LVGL Core]
           direction TB
           ImgDec[Image Decoder]:::core
           FontEng[Font Engine]:::core
           DrawUnit[Draw Unit]:::core
       end

       subgraph Cache_Framework [Cache Framework]
           direction TB

           subgraph Img_Caches [Image Caches]
               direction TB
               ImgCache["Image Cache\n(RB-LRU)"]:::cache
               HeaderCache["Header Cache\n(RB-LRU)"]:::cache
           end

           subgraph Font_Caches [Font Caches]
               direction TB
               FontCache["Face Cache\n(RB-LRU)"]:::cache
               GlyphCache["Glyph Cache\n(RB-LRU)"]:::cache
               DrawDataCache["Draw Data Cache\n(RB-LRU)"]:::cache
               KerningCache["Kerning Cache\n(RB-LRU)"]:::cache
           end

           subgraph Draw_Caches [Draw Caches]
               direction TB
               VGGradCache["VG Gradient Cache\n(LL-LRU)"]:::cache
               VGBitmapCache["VG Bitmap Cache\n(RB-LRU)"]:::cache
               SDLTextureCache["SDL Texture Cache\n(RB-LRU)"]:::cache
               GLES2TextureCache["GLES2 Texture Cache\n(RB-LRU)"]:::cache
           end
       end

       subgraph Resources [Resources]
           direction TB
           Images[Decoded Images]:::resource
           Headers[Image Headers]:::resource
           FTFace[FreeType Face]:::resource
           Glyphs[Glyph Metrics]:::resource
           DrawData[Draw Data]:::resource
           Kerning[Kerning Data]:::resource
           Gradients[Gradient Data]:::resource
           Bitmaps[Bitmap Fonts]:::resource
           Textures[GPU Textures]:::resource
       end

       %% Core to Cache Connections
       ImgDec --> ImgCache
       ImgDec --> HeaderCache

       FontEng --> FontCache
       FontEng --> GlyphCache
       FontEng --> DrawDataCache
       FontEng --> KerningCache

       DrawUnit --> VGGradCache
       DrawUnit --> VGBitmapCache
       DrawUnit --> SDLTextureCache
       DrawUnit --> GLES2TextureCache

       %% Cache to Resource Connections
       ImgCache --> Images
       HeaderCache --> Headers
       FontCache --> FTFace
       GlyphCache --> Glyphs
       DrawDataCache --> DrawData
       KerningCache --> Kerning
       VGGradCache --> Gradients
       VGBitmapCache --> Bitmaps
       SDLTextureCache --> Textures
       GLES2TextureCache --> Textures



Purpose and Benefits
********************

The main purposes of the LVGL Cache Framework are:

- **Memory Optimization**: Efficiently manage limited memory resources in embedded
  systems
- **Performance Improvement**: Reduce the overhead of repeated resource loading and
  processing
- **Resource Management**: Provide a systematic approach to resource lifecycle
  management

Key benefits include:

- **Reduced CPU Usage**: Minimizes repeated decoding of resources
- **Lower Memory Fragmentation**: Controlled allocation and deallocation of resources
- **Improved Responsiveness**: Faster access to frequently used resources
- **Consistent Performance**: Predictable behavior under various memory conditions

For more detailed information about the architecture and implementation, refer to the
:doc:`architecture` and :doc:`implementation_details` sections.
