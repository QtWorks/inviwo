/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2019 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#ifndef IVW_PARALLELCOORDINATES_H
#define IVW_PARALLELCOORDINATES_H

#include <modules/plottinggl/plottingglmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/interaction/pickingmapper.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/boolcompositeproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/minmaxproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/transferfunctionproperty.h>
#include <inviwo/core/rendering/meshdrawer.h>
#include <modules/brushingandlinking/ports/brushingandlinkingports.h>
#include <modules/plotting/datastructures/dataframe.h>
#include <modules/plotting/properties/axisproperty.h>
#include <modules/plottinggl/utils/axisrenderer.h>

#include <modules/opengl/rendering/meshdrawergl.h>
#include <modules/opengl/shader/shader.h>
#include <modules/opengl/rendering/texturequadrenderer.h>
#include <modules/fontrendering/textrenderer.h>
#include <modules/plotting/properties/dataframeproperty.h>
#include <modules/plotting/properties/marginproperty.h>

#include <modules/plottinggl/processors/parallelcoordinates/parallelcoordinatesaxissettingsproperty.h>

namespace inviwo {
class Mesh;
class PickingEvent;

/** \docpage{org.inviwo.ParallelCoordinates, Parallel Coordinates}
 * ![](org.inviwo.ParallelCoordinates.png?classIdentifier=org.inviwo.ParallelCoordinates)
 * This processor plots a given DataFrame using a Parallel Coordinate Plot.
 *
 * ### Inports
 *   * __DataFrame__  data input for plotting
 *   * __BrushingAndLinking__   inport for brushing & linking interactions
 *
 * ### Outports
 *   * __outport__   rendered image of the parallel coordinate plot
 *
 */

namespace plot {

class IVW_MODULE_PLOTTINGGL_API ParallelCoordinates : public Processor {
public:
    enum class BlendMode { None = 0, Additive = 1, Sutractive = 2, Regular = 3 };

    enum class LabelPosition { None, Above, Below };

public:
    ParallelCoordinates();
    virtual ~ParallelCoordinates();

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

protected:
    void linePicked(PickingEvent *p);
    void handlePicked(PickingEvent *p);

private:
    void createOrUpdateProperties();

    struct AxisData {
        AxisProperty *prop;
        bool upperBrushed_ = false;  //! Flag to indicated if the upper handle is brushing away data
        bool lowerBrushed_ = false;  //! Flag to indicated if the lower handle is brushing away data

        double p0_;
        double p25_;
        double p75_;
        double p100_;

        size_t columnId_;
        bool updating_ = false;
        std::function<double(size_t)> at = [](size_t) { return 0.0; };

		double getValue(double v) const {
            const auto rangeTmp = prop->range_.getRange();
            if (v <= 0) {
                return rangeTmp.x;
            }
            if (v >= 1) {
                return rangeTmp.y;
            }
            //if (!usePercentiles.get()) {
                return rangeTmp.x + v * (rangeTmp.y - rangeTmp.x);
            //} else {
            //    if (v < 0.25) {
            //        v /= 0.25;
            //        return p0_ + v * (p25_ - p0_);
            //    } else if (v < 0.75) {
            //        v -= 0.25;
            //        v /= 0.5;
            //        return p25_ + v * (p75_ - p25_);
            //    } else {
            //        v -= 0.75;
            //        v /= 0.25;
            //        return p75_ + v * (p100_ - p75_);
            //    }
            //}
        }
    };

    using ColumnAxis =
        std::tuple<std::shared_ptr<const Column>, AxisProperty *, AxisRenderer, AxisData>;
    void buildLineMesh(const std::vector<ColumnAxis*>& enabledAxis);
    void drawAxis(size2_t size,
                  const std::vector<ParallelCoordinatesAxisSettingsProperty *> &enabledAxis);
    void drawHandles(size2_t size, const std::vector<ColumnAxis*>& enabledAxis);
    void drawLines(size2_t size);

    void buildTextCache(const std::vector<ParallelCoordinatesAxisSettingsProperty *> &enabledAxis);

    void renderText(size2_t size,
                    const std::vector<ParallelCoordinatesAxisSettingsProperty *> &enabledAxis);

	void updateLayout(); // Call every time image size or margin changes
    void updateBrushing();
    double getNormalized(const AxisProperty &axis, double v) const;
    void updateFromColumn(const Column &c, AxisProperty &axis);

    DataInport<DataFrame> dataFrame_;
    BrushingAndLinkingInport brushingAndLinking_;
    ImageOutport outport_;

    CompositeProperty axisProperties_;

    CompositeProperty colors_;
    FloatVec4Property axisColor_;
    FloatVec4Property handleBaseColor_;
    FloatVec4Property handleFilteredColor_;
    TransferFunctionProperty tf_;
    TransferFunctionProperty tfSelection_;
    BoolProperty enableHoverColor_;

    CompositeProperty filteringOptions_;
    BoolProperty showFiltered_;
    FloatVec4Property filterColor_;
    FloatProperty filterIntensity_;

    ButtonProperty resetHandlePositions_;

    TemplateOptionProperty<BlendMode> blendMode_;
    FloatProperty alpha_;
    FloatProperty filterAlpha_;
    FloatProperty falllofPower_;
    FloatProperty lineWidth_;
    FloatProperty selectedLineWidth_;

    FloatVec2Property handleSize_;

    MarginProperty margins_;
    ButtonProperty autoMargins_;

    DataFrameColumnProperty selectedColorAxis_;

    CompositeProperty text_;
    TemplateOptionProperty<LabelPosition> labelPosition_;
    BoolProperty showValue_;
    FloatVec4Property color_;
    OptionPropertyInt fontSize_;
    OptionPropertyInt valuesFontSize_;

    Shader lineShader_;
    Shader axisShader_;
    Shader handleShader_;

    std::unique_ptr<Mesh> handle_;
    std::unique_ptr<MeshDrawer> handleDrawer_;

    std::unique_ptr<Mesh> axis_;
    std::unique_ptr<MeshDrawer> axisDrawer_;

    std::unique_ptr<Mesh> lines_;
    std::unique_ptr<MeshDrawerGL> linesDrawer_;

    std::vector<ColumnAxis> axisVector_;  // owned by axisProperty_

    PickingMapper linePicking_;
    PickingMapper handlePicking_;

    TextRenderer textRenderer_;
    TextureQuadRenderer textureRenderer_;

    std::shared_ptr<Image> handleImg_;

    int hoveredLine_ = -1;

    bool recreateLines_;
    bool textCacheDirty_;
    bool brushingDirty_;
};

}  // namespace plot

}  // namespace inviwo

#endif  // IVW_PARALLELCOORDINATES_H
