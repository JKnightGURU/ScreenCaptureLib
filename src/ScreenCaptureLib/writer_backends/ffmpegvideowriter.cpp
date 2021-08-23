#include "ffmpegvideowriter.h"

#include <iostream>
#include <opencv2/imgproc.hpp>

namespace sc_api {

const int MPEG4_QUALITY = 7;

void FFmpegVideoWriter::write(cv::Mat in_mat)
{
    cv::Mat mat;

    if (mat.type() == CV_8UC3)
        mat = in_mat;
    else
        cv::cvtColor(in_mat, mat, cv::COLOR_BGRA2RGB);

    // The AVFrame data will be stored as RGBRGBRGB... row-wise,
    // from left to right and from top to bottom.

    rgbpic->data[0] = mat.data;

    // Not actually scaling anything, but just converting
    // the RGB data to YUV and store it in yuvpic.
    sws_scale(swsCtx, rgbpic->data, rgbpic->linesize, 0,
              settings_.height_, yuvpic->data, yuvpic->linesize);

    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

    // The PTS of the frame are just in a reference unit,
    // unrelated to the format we are using. We set them,
    // for instance, as the corresponding frame number.
    yuvpic->pts = iframe; // * (1000 / settings_.fps_);
    iframe++;

    yuvpic->quality   = FF_QP2LAMBDA * MPEG4_QUALITY;
    yuvpic->pict_type = AV_PICTURE_TYPE_NONE;

    int got_output;
    avcodec_encode_video2(c, &pkt, yuvpic, &got_output);

    if (got_output)
    {
        fflush(stdout);

        // We set the packet PTS and DTS taking in the account our FPS (second argument),
        // and the time base that our selected format uses (third argument).
        av_packet_rescale_ts(&pkt, { 1, settings_.fps_ }, stream->time_base);

        pkt.stream_index = stream->index;

        // Write the encoded frame to the mp4 file.
        av_interleaved_write_frame(fc, &pkt);
        av_packet_unref(&pkt);
    }
}

FFmpegVideoWriter::~FFmpegVideoWriter()
{
    // Writing the delayed frames:
    for (int got_output = 1; got_output;)
    {
        avcodec_encode_video2(c, &pkt, nullptr, &got_output);
        iframe++;

        if (got_output)
        {
            fflush(stdout);
            av_packet_rescale_ts(&pkt, { 1, settings_.fps_ }, stream->time_base);
            pkt.stream_index = stream->index;
            av_interleaved_write_frame(fc, &pkt);
            av_packet_unref(&pkt);
        }
    }

    // Writing the end of the file.
    av_write_trailer(fc);

    // Closing the file.
    if (!(fmt->flags & AVFMT_NOFILE))
        avio_closep(&fc->pb);
    avcodec_close(stream->codec);

    // Freeing all the allocated memory:
    sws_freeContext(swsCtx);
    av_frame_free(&rgbpic);
    av_frame_free(&yuvpic);
    avformat_free_context(fc);
}

void FFmpegVideoWriter::initImpl()
{
    iframe = 0;

    // Preparing to convert my generated RGB images to YUV frames.
    swsCtx = sws_getContext(settings_.width_, settings_.height_,
                            AV_PIX_FMT_RGB24, settings_.width_,
                            settings_.height_, AV_PIX_FMT_YUV420P,
                            SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

    // Preparing the data concerning the format and codec,
    // in order to write properly the header, frame data and end of file.
    const char* fmtext = "mp4";

    fmt = av_guess_format(fmtext, nullptr, nullptr);
    avformat_alloc_output_context2(&fc, nullptr, nullptr, settings_.fname_);

    AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);

    AVDictionary* opt = NULL;

    stream            = avformat_new_stream(fc, codec);
    c                 = stream->codec;
    c->width          = settings_.width_;
    c->height         = settings_.height_;
    c->pix_fmt        = AV_PIX_FMT_YUV420P;
    c->time_base      = { 1, settings_.fps_ };
    c->flags         |= AV_CODEC_FLAG_QSCALE;
    c->global_quality = FF_QP2LAMBDA * MPEG4_QUALITY;

    // Setting up the format, its stream(s),
    // linking with the codec(s) and write the header.
    if (fc->oformat->flags & AVFMT_GLOBALHEADER)
        // Some formats require a global header.
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    avcodec_open2(c, codec, &opt);

    // Once the codec is set up, we need to let the container know
    // which codec are the streams using, in this case the only (video) stream.
    stream->time_base = { 1, settings_.fps_ };
    av_dump_format(fc, 0, settings_.fname_, 1);
    avio_open(&fc->pb, settings_.fname_, AVIO_FLAG_WRITE);
    int ret = avformat_write_header(fc, &opt);
    av_dict_free(&opt);

    // Preparing the containers of the frame data:
    // Allocating memory for each RGB frame, which will be lately converted to YUV.
    rgbpic         = av_frame_alloc();
    rgbpic->format = AV_PIX_FMT_RGB24;
    rgbpic->width  = settings_.width_;
    rgbpic->height = settings_.height_;
    ret            = av_frame_get_buffer(rgbpic, 1);

    // Allocating memory for each conversion output YUV frame.
    yuvpic         = av_frame_alloc();
    yuvpic->format = AV_PIX_FMT_YUV420P;
    yuvpic->width  = settings_.width_;
    yuvpic->height = settings_.height_;
    ret            = av_frame_get_buffer(yuvpic, 1);
}

}
