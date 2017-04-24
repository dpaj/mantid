// clang-format off
// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_DETSPECMAPPINGSCHEMA_ISISSTREAM_H_
#define FLATBUFFERS_GENERATED_DETSPECMAPPINGSCHEMA_ISISSTREAM_H_

#include "flatbuffers/flatbuffers.h"


namespace ISISStream {

struct SpectraDetectorMapping;

struct SpectraDetectorMapping FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_SPEC = 4,
    VT_DET = 6,
    VT_N_SPECTRA = 8
  };
  const flatbuffers::Vector<int32_t> *spec() const { return GetPointer<const flatbuffers::Vector<int32_t> *>(VT_SPEC); }
  const flatbuffers::Vector<int32_t> *det() const { return GetPointer<const flatbuffers::Vector<int32_t> *>(VT_DET); }
  int32_t n_spectra() const { return GetField<int32_t>(VT_N_SPECTRA, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_SPEC) &&
           verifier.Verify(spec()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_DET) &&
           verifier.Verify(det()) &&
           VerifyField<int32_t>(verifier, VT_N_SPECTRA) &&
           verifier.EndTable();
  }
};

struct SpectraDetectorMappingBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_spec(flatbuffers::Offset<flatbuffers::Vector<int32_t>> spec) { fbb_.AddOffset(SpectraDetectorMapping::VT_SPEC, spec); }
  void add_det(flatbuffers::Offset<flatbuffers::Vector<int32_t>> det) { fbb_.AddOffset(SpectraDetectorMapping::VT_DET, det); }
  void add_n_spectra(int32_t n_spectra) { fbb_.AddElement<int32_t>(SpectraDetectorMapping::VT_N_SPECTRA, n_spectra, 0); }
  SpectraDetectorMappingBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  SpectraDetectorMappingBuilder &operator=(const SpectraDetectorMappingBuilder &);
  flatbuffers::Offset<SpectraDetectorMapping> Finish() {
    auto o = flatbuffers::Offset<SpectraDetectorMapping>(fbb_.EndTable(start_, 3));
    return o;
  }
};

inline flatbuffers::Offset<SpectraDetectorMapping> CreateSpectraDetectorMapping(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<flatbuffers::Vector<int32_t>> spec = 0,
   flatbuffers::Offset<flatbuffers::Vector<int32_t>> det = 0,
   int32_t n_spectra = 0) {
  SpectraDetectorMappingBuilder builder_(_fbb);
  builder_.add_n_spectra(n_spectra);
  builder_.add_det(det);
  builder_.add_spec(spec);
  return builder_.Finish();
}

inline const ISISStream::SpectraDetectorMapping *GetSpectraDetectorMapping(const void *buf) { return flatbuffers::GetRoot<ISISStream::SpectraDetectorMapping>(buf); }

inline bool VerifySpectraDetectorMappingBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<ISISStream::SpectraDetectorMapping>(); }

inline void FinishSpectraDetectorMappingBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<ISISStream::SpectraDetectorMapping> root) { fbb.Finish(root); }

}  // namespace ISISStream

#endif  // FLATBUFFERS_GENERATED_DETSPECMAPPINGSCHEMA_ISISSTREAM_H_
// clang-format on