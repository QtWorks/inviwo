#include "imagegrayscale.h"

#include <inviwo/core/datastructures/imageram.h>

namespace inviwo {

ImageGrayscale::ImageGrayscale()
    : ProcessorGL(),
      inport0_(Port::INPORT, "inport0"),
      outport_(Port::OUTPORT, "outport")
      
{
    addPort(inport0_);
    addPort(outport_);
}

ImageGrayscale::~ImageGrayscale() {}

Processor* ImageGrayscale::create() const {
    return new ImageGrayscale();
}

void ImageGrayscale::initialize() {
    ProcessorGL::initialize();
    shader_ = new Shader("img_graysc.frag");
}

void ImageGrayscale::deinitialize() {
    delete shader_;
    Processor::deinitialize();
}

void ImageGrayscale::process() {    
    
    const Image* inputImage = inport0_.getData();
    Image* outImage = outport_.getEditableData();
    
    const ImageGL* inImageGL = inputImage->getRepresentation<ImageGL>();
    ImageGL* outImageGL = outImage->getEditableRepresentation<ImageGL>();

    uvec2 imageSize = inImageGL->getDimension();
    outImageGL->resize(imageSize);

    activateTarget(outport_);
    bindColorTexture(inport0_, GL_TEXTURE0);

    shader_->activate();
    shader_->setUniform("inport0_", 0);
    shader_->setUniform("dimension_", vec2(1.f / imageSize[0], 1.f / imageSize[1]) );
    renderImagePlaneQuad();
    shader_->deactivate();

    deactivateCurrentTarget();
    
}

} // namespace
