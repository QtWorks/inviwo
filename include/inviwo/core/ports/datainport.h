/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2013-2015 Inviwo Foundation
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

#ifndef IVW_DATAINPORT_H
#define IVW_DATAINPORT_H

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/ports/inport.h>
#include <inviwo/core/ports/outport.h>
#include <inviwo/core/ports/outportiterator.h>
#include <inviwo/core/datastructures/data.h>
#include <inviwo/core/util/stdextensions.h>
#include <limits>
#include <iterator>

namespace inviwo {

template <typename T>
class DataOutport;

template <typename T, size_t N = 1, bool Flat = false>
class DataInport : public Inport {
public:
    DataInport(std::string identifier);
    virtual ~DataInport();

    virtual uvec3 getColorCode() const override;
    virtual std::string getClassIdentifier() const override;
    virtual std::string getContentInfo() const override;
    virtual size_t getMaxNumberOfConnections() const override;

    virtual bool canConnectTo(const Port* port) const override;
    virtual void connectTo(Outport* port) override;
    virtual bool isConnected() const override;

    virtual const T* getData() const;
    virtual std::vector<const T*> getVectorData() const;
    virtual std::vector<std::pair<Outport*, const T*>> getSourceVectorData() const;

    bool hasData() const;

    class const_iterator;
    const_iterator begin() const {
        return const_iterator(connectedOutports_.begin(), connectedOutports_.end());
    }
    const_iterator end() const {
        return const_iterator(connectedOutports_.end(), connectedOutports_.end());
    }
};

template <typename T, size_t N, bool Flat>
DataInport<T, N, Flat>::DataInport(std::string identifier)
    : Inport(identifier) {}

template <typename T, size_t N, bool Flat>
DataInport<T, N, Flat>::~DataInport() {}

template <typename T, size_t N, bool Flat>
std::string inviwo::DataInport<T, N, Flat>::getClassIdentifier() const {
    switch (N) {
        case 0:
            return port_traits<T>::class_identifier() + (Flat ? "Flat" : "") + "Inport";
        case 1:
            return port_traits<T>::class_identifier() + (Flat ? "Flat" : "") + "MultiInport";
        default:
            return port_traits<T>::class_identifier() + (Flat ? "Flat" : "") + toString(N) +
                   "Inport";
    }
}

template <typename T, size_t N, bool Flat>
uvec3 DataInport<T, N, Flat>::getColorCode() const {
    return port_traits<T>::color_code();
}

template <typename T, size_t N, bool Flat>
size_t inviwo::DataInport<T, N, Flat>::getMaxNumberOfConnections() const {
    if (N == 0)
        return std::numeric_limits<size_t>::max();
    else
        return N;
}

template <typename T, size_t N, bool Flat>
bool DataInport<T, N, Flat>::canConnectTo(const Port* port) const {
    if (port->getProcessor() == getProcessor()) return false;
    
    if (dynamic_cast<const DataOutport<T>*>(port))
        return true;
    else if (Flat && dynamic_cast<const OutportIterator<T>*>(port))
        return true;
    else
        return false;
}

template <typename T, size_t N, bool Flat>
void DataInport<T, N, Flat>::connectTo(Outport* port) {
    if (!port) return;

    const DataOutport<T>* dataPort = dynamic_cast<const DataOutport<T>*>(port);
    const OutportIterator<T>* flatPort =
        Flat ? dynamic_cast<const OutportIterator<T>*>(port) : nullptr;

    if (dataPort == nullptr && flatPort == nullptr)
        throw Exception("Trying to connect incompatible ports.", IvwContext);

    if (getNumberOfConnections() + 1 > getMaxNumberOfConnections())
        throw Exception("Trying to connect to a full port.", IvwContext);

    Inport::connectTo(port);
}

template <typename T, size_t N, bool Flat>
bool inviwo::DataInport<T, N, Flat>::isConnected() const {
    if (N == 0)
        return !connectedOutports_.empty();
    else
        return connectedOutports_.size() >= 1 && connectedOutports_.size() <= N;
}

template <typename T, size_t N, bool Flat>
bool DataInport<T, N, Flat>::hasData() const {
    return isConnected() && util::all_of(connectedOutports_, [](Outport* p) {
        return static_cast<DataOutport<T>*>(p)->hasData();
    });
}

template <typename T, size_t N, bool Flat>
const T* DataInport<T, N, Flat>::getData() const {
    if (isConnected()) {
        // Safe to static cast since we are unable to connect other outport types.
        return &*begin();
    } else {
        return nullptr;
    }
}

template <typename T, size_t N, bool Flat>
std::vector<const T*> DataInport<T, N, Flat>::getVectorData() const {
    std::vector<const T*> res(N);

    for (auto it = this->begin(); it!= this->end(); ++it) res.push_back(&*it);
    
    return res;
}

template <typename T, size_t N, bool Flat>
std::vector<std::pair<Outport*, const T*>> inviwo::DataInport<T, N, Flat>::getSourceVectorData() const {
    std::vector<std::pair<Outport*, const T*>> res(N);
    
    for (auto outport : connectedOutports_) {
        // Safe to static cast since we are unable to connect other outport types.
        
        if (Flat) {
            auto oi = dynamic_cast<OutportIterator<T>*>(outport);
            if (oi) {
                for (auto& elem : *oi) res.emplace_back(outport, &elem);
            }
        } else {
            auto dataport = static_cast<DataOutport<T>*>(outport);
            if (dataport->hasData()) res.emplace_back(dataport, dataport->getConstData());
        }
    }

    return res;
}

template <typename T, size_t N, bool Flat>
std::string DataInport<T, N, Flat>::getContentInfo() const {
    if (hasData()) {
        std::string info = port_traits<T>::data_info(getData());
        if (!info.empty()) {
            return info;
        } else {
            return "No information available for: " + util::class_identifier<T>();
        }
    } else {
        return "Port has no data";
    }
}

template <typename T, size_t N, bool Flat>
class DataInport<T, N, Flat>::const_iterator : public std::iterator<std::forward_iterator_tag, T> {
    using self = const_iterator;
    using PortIter = typename std::vector<Outport*>::const_iterator;
    using DataIter = typename OutportIterator<T>::const_iterator;

public:
    explicit const_iterator(PortIter pIterBegin, PortIter pIterEnd)
        : pIter_(pIterBegin), pEnd_(pIterEnd), dIter_() {
        if (Flat && pIter_ != pEnd_) dIter_ = dynamic_cast<OutportIterator<T>*>(*pIter_)->begin();
    }
    self& operator++() {
        if (Flat) {
            dIter_++;
            auto outportIterable = dynamic_cast<OutportIterator<T>*>(*pIter_);
            if (outportIterable && dIter_ == outportIterable->end()) {
                pIter_++;
                if (pIter_ != pEnd_)
                    dIter_ = outportIterable->begin();
                else {
                    dIter_ = DataIter();
                }
            }
        } else {
            pIter_++;
        }
        return *this;
    }
    self operator++(int) {
        self i = *this;

        if (Flat) {
            dIter_++;
            auto outportIterable = dynamic_cast<OutportIterator<T>*>(*pIter_);
            if (outportIterable && dIter_ == outportIterable->end()) {
                pIter_++;
                if (pIter_ != pEnd_)
                    dIter_ = dynamic_cast<OutportIterator<T>*>(*pIter_)->begin();
                else {
                    dIter_ = DataIter();
                }
            }
        } else {
            pIter_++;
        }

        return i;
    }
    const T& operator*() {
        return Flat ? *dIter_ : *(static_cast<DataOutport<T>*>(*pIter_)->getConstData());
    }
    const T* operator->() {
        return Flat ? &(*dIter_) : static_cast<DataOutport<T>*>(*pIter_)->getConstData();
    }
    bool operator==(const self& rhs) const {
        if (Flat)
            return pIter_ == rhs.pIter_ && dIter_ == rhs.dIter_;
        else
            return pIter_ == rhs.pIter_;
    }
    bool operator!=(const self& rhs) const {
        if (Flat)
            return pIter_ != rhs.pIter_ || dIter_ != rhs.dIter_;
        else
            return pIter_ != rhs.pIter_;
    }

private:
    PortIter pIter_;
    PortIter pEnd_;
    DataIter dIter_;
};

}  // namespace

#endif  // IVW_DATAINPORT_H
