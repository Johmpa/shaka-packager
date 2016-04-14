// Copyright 2016 Google Inc. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef PACKAGER_MEDIA_FORMATS_MP2T_TS_WRITER_H_
#define PACKAGER_MEDIA_FORMATS_MP2T_TS_WRITER_H_

#include <list>
#include <map>
#include <vector>

#include "packager/base/memory/scoped_ptr.h"
#include "packager/media/base/media_stream.h"
#include "packager/media/file/file.h"
#include "packager/media/file/file_closer.h"
#include "packager/media/formats/mp2t/continuity_counter.h"
#include "packager/media/formats/mp2t/pes_packet.h"
#include "packager/media/formats/mp2t/program_map_table_writer.h"

namespace edash_packager {
namespace media {
namespace mp2t {

/// This class takes PesPackets, encapsulates them into TS packets, and write
/// the data to file. This also creates PSI from StreamInfo.
class TsWriter {
 public:
  TsWriter();
  virtual ~TsWriter();

  /// This must be called before calling other methods.
  /// @param stream_info is the information about this stream.
  /// @param will_be_encrypted must be true if some segment would be encrypted.
  ///        It is ok if the entire stream is not encrypted but have this true
  ///        e.g. if the clear lead is very long.
  /// @return true on success, false otherwise.
  virtual bool Initialize(const StreamInfo& stream_info,
                          bool will_be_encrypted);

  /// This will fail if the current segment is not finalized.
  /// @param file_name is the output file name.
  /// @param encrypted must be true if the new segment is encrypted.
  /// @return true on success, false otherwise.
  virtual bool NewSegment(const std::string& file_name);

  /// Signals the writer that the rest of the segments are encrypted.
  /// |will_be_encrypted| passed to Initialize() should be true.
  void SignalEncypted();

  /// Flush all the pending PesPackets that have not been written to file and
  /// close the file.
  /// @return true on success, false otherwise.
  virtual bool FinalizeSegment();

  /// Add PesPacket to the instance. PesPacket might not get written to file
  /// immediately.
  /// @param pes_packet gets added to the writer.
  /// @return true on success, false otherwise.
  virtual bool AddPesPacket(scoped_ptr<PesPacket> pes_packet);

  /// Only for testing.
  void SetProgramMapTableWriterForTesting(
      scoped_ptr<ProgramMapTableWriter> table_writer);

 private:
  // True if further segments generated by this instance should be encrypted.
  bool encrypted_ = false;
  // The stream will be encrypted some time later.
  bool will_be_encrypted_ = false;

  ContinuityCounter pmt_continuity_counter_;
  ContinuityCounter pat_continuity_counter_;
  ContinuityCounter elementary_stream_continuity_counter_;

  scoped_ptr<ProgramMapTableWriter> pmt_writer_;

  scoped_ptr<File, FileCloser> current_file_;

  DISALLOW_COPY_AND_ASSIGN(TsWriter);
};

}  // namespace mp2t
}  // namespace media
}  // namespace edash_packager

#endif  // PACKAGER_MEDIA_FORMATS_MP2T_TS_WRITER_H_
