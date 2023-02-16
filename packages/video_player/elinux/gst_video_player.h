// Copyright 2021 Sony Group Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_ELINUX_GST_VIDEO_PLAYER_H_
#define PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_ELINUX_GST_VIDEO_PLAYER_H_

#include <gst/gst.h>

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <regex>
#include <vector>

#include "video_player_stream_handler.h"

class GstVideoPlayer {
 public:
  GstVideoPlayer(const std::string& uri,
                 std::unique_ptr<VideoPlayerStreamHandler> handler);
  ~GstVideoPlayer();

  static void GstLibraryLoad();
  static void GstLibraryUnload();

  bool Play();
  bool Pause();
  bool Stop();
  bool SetVolume(double volume);
  bool SetPlaybackRate(double rate);
  void SetAutoRepeat(bool auto_repeat) { auto_repeat_ = auto_repeat; };
  bool SetSeek(int64_t position);
  int64_t GetDuration();
  int64_t GetCurrentPosition();
  const uint8_t* GetFrameBuffer();
  int32_t GetWidth() const { return width_; };
  int32_t GetHeight() const { return height_; };

 private:
  struct GstVideoElements {
    GstElement* pipeline;
    GstElement* video_src;
    GstElement* video_convert;
    GstElement* caps_filter;
    GstElement* video_sink;
    GstElement* output;

    GstBus* bus;
    GstBuffer* buffer;
  };

  static void HandoffHandler(GstElement* fakesink, GstBuffer* buf,
                             GstPad* new_pad, gpointer user_data);
  static GstBusSyncReply HandleGstMessage(GstBus* bus, GstMessage* message,
                                          gpointer user_data);
  std::string ParseUri(const std::string& uri);
  bool CreatePipeline();
  bool CheckPluginAvailability(const std::string & element);
  void IncreasePluginRank(const std::string & element);
  void CorrectAspectRatio();
  void DestroyPipeline();
  void Preroll();
  void GetVideoSize(int32_t& width, int32_t& height);
  bool IsStreamUri(const std::string &uri) const;
  bool SetStreamDataFromUrl(const std::string &uri);
  int NormalizeResolutionValue(const int res_val);

  GstVideoElements gst_;
  std::string uri_;
  std::string aspect_ratio_;
  std::unique_ptr<uint32_t> pixels_;
  int32_t width_;
  int32_t height_;
  double volume_ = 1.0;
  double playback_rate_ = 1.0;
  bool mute_ = false;
  bool is_stream_ = false;
  bool auto_repeat_ = false;
  bool is_completed_ = false;
  std::mutex mutex_event_completed_;
  std::shared_mutex mutex_buffer_;
  std::unique_ptr<VideoPlayerStreamHandler> stream_handler_;

  static inline auto const stream_type_regex_ {std::regex("((?:rtp|rtmp|rtcp|rtsp|udp)://.*)", std::regex::icase)};
  static inline auto const stream_ext_regex_ {std::regex("((?:http|https)://.*(?:.m3u8|.flv))", std::regex::icase)};
  static inline auto const camera_path_regex_ {std::regex("(/dev/video[0-9])", std::regex::icase)};
  const std::vector < int > resolution_values_ {1080,1920,2160,3480};
};

#endif  // PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_ELINUX_GST_VIDEO_PLAYER_H_
