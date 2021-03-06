// Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "update_engine/prefs.h"

#include <base/file_util.h>
#include <base/logging.h>
#include <base/string_number_conversions.h>
#include <base/string_util.h>

#include "update_engine/utils.h"

using std::string;

namespace chromeos_update_engine {

const char kPrefsCertificateReportToSendDownload[] =
    "certificate-report-to-send-download";
const char kPrefsCertificateReportToSendUpdate[] =
    "certificate-report-to-send-update";
const char kPrefsDeltaUpdateFailures[] = "delta-update-failures";
const char kPrefsLastActivePingDay[] = "last-active-ping-day";
const char kPrefsLastRollCallPingDay[] = "last-roll-call-ping-day";
const char kPrefsManifestMetadataSize[] = "manifest-metadata-size";
const char kPrefsPreviousVersion[] = "previous-version";
const char kPrefsResumedUpdateFailures[] = "resumed-update-failures";
const char kPrefsUpdateCheckResponseHash[] = "update-check-response-hash";
const char kPrefsUpdateServerCertificate[] = "update-server-cert";
const char kPrefsUpdateStateNextDataOffset[] = "update-state-next-data-offset";
const char kPrefsUpdateStateNextOperation[] = "update-state-next-operation";
const char kPrefsUpdateStateSHA256Context[] = "update-state-sha-256-context";
const char kPrefsUpdateStateSignatureBlob[] = "update-state-signature-blob";
const char kPrefsUpdateStateSignedSHA256Context[] =
    "update-state-signed-sha-256-context";
const char kPrefsUpdateCheckCount[] = "update-check-count";
const char kPrefsWallClockWaitPeriod[] = "wall-clock-wait-period";
const char kPrefsUpdateFirstSeenAt[] = "update-first-seen-at";

const char kPrefsPayloadAttemptNumber[] = "payload-attempt-number";
const char kPrefsCurrentResponseSignature[] = "current-response-signature";
const char kPrefsCurrentUrlIndex[] = "current-url-index";
const char kPrefsCurrentUrlFailureCount[] = "current-url-failure-count";
const char kPrefsBackoffExpiryTime[] = "backoff-expiry-time";

bool Prefs::Init(const FilePath& prefs_dir) {
  prefs_dir_ = prefs_dir;
  return true;
}

bool Prefs::GetString(const string& key, string* value) {
  FilePath filename;
  TEST_AND_RETURN_FALSE(GetFileNameForKey(key, &filename));
  if (!file_util::ReadFileToString(filename, value)) {
    LOG(INFO) << key << " not present in " << prefs_dir_.value();
    return false;
  }
  return true;
}

bool Prefs::SetString(const std::string& key, const std::string& value) {
  FilePath filename;
  TEST_AND_RETURN_FALSE(GetFileNameForKey(key, &filename));
  TEST_AND_RETURN_FALSE(file_util::CreateDirectory(filename.DirName()));
  TEST_AND_RETURN_FALSE(
      file_util::WriteFile(filename, value.data(), value.size()) ==
      static_cast<int>(value.size()));
  return true;
}

bool Prefs::GetInt64(const string& key, int64_t* value) {
  string str_value;
  if (!GetString(key, &str_value))
    return false;
  TrimWhitespaceASCII(str_value, TRIM_ALL, &str_value);
  TEST_AND_RETURN_FALSE(base::StringToInt64(str_value, value));
  return true;
}

bool Prefs::SetInt64(const string& key, const int64_t value) {
  return SetString(key, base::Int64ToString(value));
}

bool Prefs::Exists(const string& key) {
  FilePath filename;
  TEST_AND_RETURN_FALSE(GetFileNameForKey(key, &filename));
  return file_util::PathExists(filename);
}

bool Prefs::Delete(const string& key) {
  FilePath filename;
  TEST_AND_RETURN_FALSE(GetFileNameForKey(key, &filename));
  return file_util::Delete(filename, false);
}

bool Prefs::GetFileNameForKey(const std::string& key, FilePath* filename) {
  // Allows only non-empty keys containing [A-Za-z0-9_-].
  TEST_AND_RETURN_FALSE(!key.empty());
  for (size_t i = 0; i < key.size(); ++i) {
    char c = key.at(i);
    TEST_AND_RETURN_FALSE(IsAsciiAlpha(c) || IsAsciiDigit(c) ||
                          c == '_' || c == '-');
  }
  *filename = prefs_dir_.Append(key);
  return true;
}

}  // namespace chromeos_update_engine
