cmake_minimum_required(VERSION 3.20)
project(dosbox-x VERSION 2026.05.02 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ── Options ──────────────────────────────────────────────────────────────────
option(USE_SDL2       "Build with SDL2 (ON) or SDL1 (OFF)"         ON)
option(ENABLE_DEBUG   "Enable internal debugger (requires curses)" ON)
option(ENABLE_OPENGL  "Enable OpenGL output"                       ON)
option(ENABLE_D3D     "Enable Direct3D output (Windows only)"      ON)
option(ENABLE_MT32    "Enable MT-32 emulation"                     ON)
option(ENABLE_XBRZ    "Enable xBRZ scaler"                         ON)
option(ENABLE_PCAP    "Enable ethernet pass-through (libpcap)"     ON)
option(ENABLE_PRINTER "Enable printer emulation"                   ON)
option(ENABLE_FPU     "Enable FPU emulation"                       ON)
option(ENABLE_GAMELINK "Enable gamelink support (SDL2 only)"       ON)

# ── Platform detection ───────────────────────────────────────────────────────
if(WIN32)
    set(IS_WINDOWS TRUE)
elseif(APPLE)
    set(IS_MACOS TRUE)
else()
    set(IS_LINUX TRUE)
endif()

# ── CPU target detection ────────────────────────────────────────────────────
if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|x86_64|x86|i[3-6]86")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(DOSBOX_CPU_TARGET "X86_64")
        set(CAN_DYNAMIC_X86 TRUE)
    else()
        set(DOSBOX_CPU_TARGET "X86")
        set(CAN_DYNAMIC_X86 TRUE)
    endif()
    set(CAN_DYNREC TRUE)
    set(CAN_FPU_X86 TRUE)
    set(CAN_MT32 TRUE)
    set(CAN_UNALIGNED TRUE)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64|arm64")
    set(DOSBOX_CPU_TARGET "ARMV8LE")
    set(CAN_DYNAMIC_X86 FALSE)
    set(CAN_DYNREC TRUE)
    set(CAN_FPU_X86 FALSE)
    set(CAN_MT32 TRUE)
    set(CAN_UNALIGNED TRUE)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    set(DOSBOX_CPU_TARGET "ARMV7LE")
    set(CAN_DYNAMIC_X86 FALSE)
    set(CAN_DYNREC TRUE)
    set(CAN_FPU_X86 FALSE)
    set(CAN_MT32 TRUE)
    set(CAN_UNALIGNED TRUE)
else()
    set(DOSBOX_CPU_TARGET "UNKNOWN")
    set(CAN_DYNAMIC_X86 FALSE)
    set(CAN_DYNREC FALSE)
    set(CAN_FPU_X86 FALSE)
    set(CAN_MT32 FALSE)
    set(CAN_UNALIGNED FALSE)
endif()

# ── Source files ─────────────────────────────────────────────────────────────
set(DOSBOX_SOURCES
    src/dosbox.cpp

    # aviwriter
    src/aviwriter/avi_rw_iobuf.cpp
    src/aviwriter/avi_writer.cpp
    src/aviwriter/guid.cpp
    src/aviwriter/ksdataformat.cpp
    src/aviwriter/riff.cpp
    src/aviwriter/riff_wav_writer.cpp

    # builtin
    src/builtin/25_com.cpp
    src/builtin/28_com.cpp
    src/builtin/4dos.cpp
    src/builtin/50_com.cpp
    src/builtin/append_exe.cpp
    src/builtin/buffers_com.cpp
    src/builtin/cdplay.cpp
    src/builtin/chkdsk_exe.cpp
    src/builtin/cwsdpmi_exe.cpp
    src/builtin/cwsdpmi_exe_pc98.cpp
    src/builtin/debug_exe.cpp
    src/builtin/defrag_exe.cpp
    src/builtin/device_com.cpp
    src/builtin/diskcopy_exe.cpp
    src/builtin/dos32a_exe.cpp
    src/builtin/dos4gw_exe.cpp
    src/builtin/dosidle_exe.cpp
    src/builtin/dosmid.cpp
    src/builtin/dsxmenu_exe_pc.cpp
    src/builtin/dsxmenu_exe_pc98.cpp
    src/builtin/edit_com.cpp
    src/builtin/emsmagic.cpp
    src/builtin/eval.cpp
    src/builtin/fc_exe.cpp
    src/builtin/fcbs_com.cpp
    src/builtin/fdisk_exe.cpp
    src/builtin/files_com.cpp
    src/builtin/find_exe.cpp
    src/builtin/format_exe.cpp
    src/builtin/glide2x.cpp
    src/builtin/hexmem16_exe.cpp
    src/builtin/hexmem32_exe.cpp
    src/builtin/ipsmake.cpp
    src/builtin/ipspatch.cpp
    src/builtin/lastdriv_com.cpp
    src/builtin/mem_com.cpp
    src/builtin/move_exe.cpp
    src/builtin/mpxplay.cpp
    src/builtin/ne2000bin.cpp
    src/builtin/print_com.cpp
    src/builtin/replace_exe.cpp
    src/builtin/shutdown.cpp
    src/builtin/sort_exe.cpp
    src/builtin/sys_com.cpp
    src/builtin/textutil.cpp
    src/builtin/xcopy_exe.cpp
    src/builtin/zip.cpp

    # cpu
    src/cpu/callback.cpp
    src/cpu/core_dyn_x86.cpp
    src/cpu/core_dynrec.cpp
    src/cpu/core_full.cpp
    src/cpu/core_normal.cpp
    src/cpu/core_normal_286.cpp
    src/cpu/core_normal_8086.cpp
    src/cpu/core_prefetch.cpp
    src/cpu/core_prefetch_286.cpp
    src/cpu/core_prefetch_8086.cpp
    src/cpu/core_simple.cpp
    src/cpu/cpu.cpp
    src/cpu/flags.cpp
    src/cpu/mmx.cpp
    src/cpu/modrm.cpp
    src/cpu/paging.cpp

    # debug
    src/debug/debug.cpp
    src/debug/debug_disasm.cpp
    src/debug/debug_gui.cpp

    # dos
    src/dos/cdrom.cpp
    src/dos/cdrom_image.cpp
    src/dos/dos.cpp
    src/dos/dos_classes.cpp
    src/dos/dos_devices.cpp
    src/dos/dos_execute.cpp
    src/dos/dos_files.cpp
    src/dos/dos_ioctl.cpp
    src/dos/dos_keyboard_layout.cpp
    src/dos/dos_memory.cpp
    src/dos/dos_misc.cpp
    src/dos/dos_mscdex.cpp
    src/dos/dos_programs.cpp
    src/dos/dos_tables.cpp
    src/dos/drive_cache.cpp
    src/dos/drive_fat.cpp
    src/dos/drive_iso.cpp
    src/dos/drive_local.cpp
    src/dos/drive_overlay.cpp
    src/dos/drive_physfs.cpp
    src/dos/drive_virtual.cpp
    src/dos/drives.cpp
    src/dos/eltorito.cpp

    # fpu
    src/fpu/fpu.cpp

    # gui
    src/gui/bitop.cpp
    src/gui/menu.cpp
    src/gui/menu_callback.cpp
    src/gui/midi.cpp
    src/gui/ptrop.cpp
    src/gui/render.cpp
    src/gui/render_scalers.cpp
    src/gui/sdl_gui.cpp
    src/gui/sdl_mapper.cpp
    src/gui/sdlmain.cpp
    src/gui/zipcrc.c
    src/gui/zipfile.cpp

    # hardware
    src/hardware/8255.cpp
    src/hardware/adlib.cpp
    src/hardware/cmos.cpp
    src/hardware/dbopl.cpp
    src/hardware/disney.cpp
    src/hardware/dma.cpp
    src/hardware/dongle.cpp
    src/hardware/floppy.cpp
    src/hardware/gameblaster.cpp
    src/hardware/glide.cpp
    src/hardware/gus.cpp
    src/hardware/hardopl.cpp
    src/hardware/hardware.cpp
    src/hardware/ide.cpp
    src/hardware/imfc.cpp
    src/hardware/imfc_rom.c
    src/hardware/innova.cpp
    src/hardware/iohandler.cpp
    src/hardware/ipx.cpp
    src/hardware/ipxserver.cpp
    src/hardware/joystick.cpp
    src/hardware/keyboard.cpp
    src/hardware/memory.cpp
    src/hardware/mixer.cpp
    src/hardware/mpu401.cpp
    src/hardware/ne2000.cpp
    src/hardware/nukedopl.cpp
    src/hardware/opl.cpp
    src/hardware/pc98.cpp
    src/hardware/pc98_artic.cpp
    src/hardware/pc98_fm.cpp
    src/hardware/pci_bus.cpp
    src/hardware/pcspeaker.cpp
    src/hardware/pic.cpp
    src/hardware/ps1_sound.cpp
    src/hardware/sblaster.cpp
    src/hardware/tandy_sound.cpp
    src/hardware/timer.cpp
    src/hardware/vga.cpp
    src/hardware/vga_ati.cpp
    src/hardware/vga_attr.cpp
    src/hardware/vga_crtc.cpp
    src/hardware/vga_dac.cpp
    src/hardware/vga_draw.cpp
    src/hardware/vga_gfx.cpp
    src/hardware/vga_memory.cpp
    src/hardware/vga_misc.cpp
    src/hardware/vga_other.cpp
    src/hardware/vga_paradise.cpp
    src/hardware/vga_pc98_cg.cpp
    src/hardware/vga_pc98_crtc.cpp
    src/hardware/vga_pc98_dac.cpp
    src/hardware/vga_pc98_egc.cpp
    src/hardware/vga_pc98_gdc.cpp
    src/hardware/vga_pc98_gdc_draw.cpp
    src/hardware/vga_s3.cpp
    src/hardware/vga_seq.cpp
    src/hardware/vga_tseng.cpp
    src/hardware/vga_xga.cpp
    src/hardware/voodoo.cpp
    src/hardware/voodoo_emu.cpp
    src/hardware/voodoo_interface.cpp
    src/hardware/voodoo_opengl.cpp
    src/hardware/voodoo_vogl.cpp

    # hardware/mame
    src/hardware/mame/fmopl.cpp
    src/hardware/mame/saa1099.cpp
    src/hardware/mame/sn76496.cpp
    src/hardware/mame/ymdeltat.cpp
    src/hardware/mame/ymf262.cpp

    # hardware/opl boards
    src/hardware/opl2board/opl2board.cpp
    src/hardware/opl3duoboard/opl3duoboard.cpp

    # hardware/reSID
    src/hardware/reSID/envelope.cpp
    src/hardware/reSID/extfilt.cpp
    src/hardware/reSID/filter.cpp
    src/hardware/reSID/pot.cpp
    src/hardware/reSID/sid.cpp
    src/hardware/reSID/version.cpp
    src/hardware/reSID/voice.cpp
    src/hardware/reSID/wave.cpp
    src/hardware/reSID/wave6581_PST.cpp
    src/hardware/reSID/wave6581_PS_.cpp
    src/hardware/reSID/wave6581_P_T.cpp
    src/hardware/reSID/wave6581__ST.cpp
    src/hardware/reSID/wave8580_PST.cpp
    src/hardware/reSID/wave8580_PS_.cpp
    src/hardware/reSID/wave8580_P_T.cpp
    src/hardware/reSID/wave8580__ST.cpp

    # hardware/serialport
    src/hardware/serialport/directserial.cpp
    src/hardware/serialport/libserial.cpp
    src/hardware/serialport/misc_util.cpp
    src/hardware/serialport/nullmodem.cpp
    src/hardware/serialport/serialdummy.cpp
    src/hardware/serialport/serialfile.cpp
    src/hardware/serialport/seriallog.cpp
    src/hardware/serialport/serialmouse.cpp
    src/hardware/serialport/serialport.cpp
    src/hardware/serialport/softmodem.cpp

    # hardware/parport
    src/hardware/parport/directlpt.cpp
    src/hardware/parport/filelpt.cpp
    src/hardware/parport/parport.cpp
    src/hardware/parport/printer.cpp
    src/hardware/parport/printer_charmaps.cpp
    src/hardware/parport/printer_redir.cpp

    # hardware/RetroWaveLib
    src/hardware/RetroWaveLib/Board/OPL3.c
    src/hardware/RetroWaveLib/Protocol/Serial.c
    src/hardware/RetroWaveLib/RetroWave.c
    src/hardware/RetroWaveLib/RetroWave_DOSBoX.cpp

    # hardware/esfmu
    src/hardware/esfmu/esfm.c
    src/hardware/esfmu/esfm_registers.c

    # hardware/snd_pc98
    src/hardware/snd_pc98/cbus/board26k.c
    src/hardware/snd_pc98/cbus/board86.c
    src/hardware/snd_pc98/cbus/pcm86io.c
    src/hardware/snd_pc98/common/parts.c
    src/hardware/snd_pc98/generic/keydisp.c
    src/hardware/snd_pc98/sound/adpcmc.c
    src/hardware/snd_pc98/sound/adpcmg.c
    src/hardware/snd_pc98/sound/fmboard.c
    src/hardware/snd_pc98/sound/fmtimer.c
    src/hardware/snd_pc98/sound/getsnd/getsmix.c
    src/hardware/snd_pc98/sound/getsnd/getsnd.c
    src/hardware/snd_pc98/sound/getsnd/getwave.c
    src/hardware/snd_pc98/sound/opngenc.c
    src/hardware/snd_pc98/sound/opngeng.c
    src/hardware/snd_pc98/sound/pcm86c.c
    src/hardware/snd_pc98/sound/pcm86g.c
    src/hardware/snd_pc98/sound/psggenc.c
    src/hardware/snd_pc98/sound/psggeng.c
    src/hardware/snd_pc98/sound/rhythmc.c
    src/hardware/snd_pc98/sound/sound.c
    src/hardware/snd_pc98/sound/soundrom.c
    src/hardware/snd_pc98/sound/tms3631c.c
    src/hardware/snd_pc98/sound/tms3631g.c
    src/hardware/snd_pc98/x11/dosio.c

    # ints
    src/ints/bios.cpp
    src/ints/bios_disk.cpp
    src/ints/bios_keyboard.cpp
    src/ints/bios_memdisk.cpp
    src/ints/bios_vhd.cpp
    src/ints/ems.cpp
    src/ints/int10.cpp
    src/ints/int10_char.cpp
    src/ints/int10_memory.cpp
    src/ints/int10_misc.cpp
    src/ints/int10_modes.cpp
    src/ints/int10_pal.cpp
    src/ints/int10_put_pixel.cpp
    src/ints/int10_vesa.cpp
    src/ints/int10_video_state.cpp
    src/ints/int10_vptable.cpp
    src/ints/int_dosv.cpp
    src/ints/mouse.cpp
    src/ints/pc98_lio.cpp
    src/ints/qcow2_disk.cpp
    src/ints/xms.cpp

    # libs/decoders/internal - ogg
    src/libs/decoders/internal/ogg/bitwise.c
    src/libs/decoders/internal/ogg/framing.c

    # libs/decoders/internal - opus/celt
    src/libs/decoders/internal/opus/celt/bands.c
    src/libs/decoders/internal/opus/celt/celt.c
    src/libs/decoders/internal/opus/celt/celt_decoder.c
    src/libs/decoders/internal/opus/celt/celt_encoder.c
    src/libs/decoders/internal/opus/celt/celt_lpc.c
    src/libs/decoders/internal/opus/celt/cwrs.c
    src/libs/decoders/internal/opus/celt/entcode.c
    src/libs/decoders/internal/opus/celt/entdec.c
    src/libs/decoders/internal/opus/celt/entenc.c
    src/libs/decoders/internal/opus/celt/kiss_fft.c
    src/libs/decoders/internal/opus/celt/laplace.c
    src/libs/decoders/internal/opus/celt/mathops.c
    src/libs/decoders/internal/opus/celt/mdct.c
    src/libs/decoders/internal/opus/celt/modes.c
    src/libs/decoders/internal/opus/celt/pitch.c
    src/libs/decoders/internal/opus/celt/quant_bands.c
    src/libs/decoders/internal/opus/celt/rate.c
    src/libs/decoders/internal/opus/celt/vq.c

    # libs/decoders/internal - opus/silk
    src/libs/decoders/internal/opus/silk/A2NLSF.c
    src/libs/decoders/internal/opus/silk/CNG.c
    src/libs/decoders/internal/opus/silk/HP_variable_cutoff.c
    src/libs/decoders/internal/opus/silk/LPC_analysis_filter.c
    src/libs/decoders/internal/opus/silk/LPC_fit.c
    src/libs/decoders/internal/opus/silk/LPC_inv_pred_gain.c
    src/libs/decoders/internal/opus/silk/LP_variable_cutoff.c
    src/libs/decoders/internal/opus/silk/NLSF2A.c
    src/libs/decoders/internal/opus/silk/NLSF_VQ.c
    src/libs/decoders/internal/opus/silk/NLSF_VQ_weights_laroia.c
    src/libs/decoders/internal/opus/silk/NLSF_decode.c
    src/libs/decoders/internal/opus/silk/NLSF_del_dec_quant.c
    src/libs/decoders/internal/opus/silk/NLSF_encode.c
    src/libs/decoders/internal/opus/silk/NLSF_stabilize.c
    src/libs/decoders/internal/opus/silk/NLSF_unpack.c
    src/libs/decoders/internal/opus/silk/NSQ.c
    src/libs/decoders/internal/opus/silk/NSQ_del_dec.c
    src/libs/decoders/internal/opus/silk/PLC.c
    src/libs/decoders/internal/opus/silk/VAD.c
    src/libs/decoders/internal/opus/silk/VQ_WMat_EC.c
    src/libs/decoders/internal/opus/silk/ana_filt_bank_1.c
    src/libs/decoders/internal/opus/silk/biquad_alt.c
    src/libs/decoders/internal/opus/silk/bwexpander.c
    src/libs/decoders/internal/opus/silk/bwexpander_32.c
    src/libs/decoders/internal/opus/silk/check_control_input.c
    src/libs/decoders/internal/opus/silk/code_signs.c
    src/libs/decoders/internal/opus/silk/control_SNR.c
    src/libs/decoders/internal/opus/silk/control_audio_bandwidth.c
    src/libs/decoders/internal/opus/silk/control_codec.c
    src/libs/decoders/internal/opus/silk/debug_silk.c
    src/libs/decoders/internal/opus/silk/dec_API.c
    src/libs/decoders/internal/opus/silk/decode_core.c
    src/libs/decoders/internal/opus/silk/decode_frame.c
    src/libs/decoders/internal/opus/silk/decode_indices.c
    src/libs/decoders/internal/opus/silk/decode_parameters.c
    src/libs/decoders/internal/opus/silk/decode_pitch.c
    src/libs/decoders/internal/opus/silk/decode_pulses.c
    src/libs/decoders/internal/opus/silk/decoder_set_fs.c
    src/libs/decoders/internal/opus/silk/enc_API.c
    src/libs/decoders/internal/opus/silk/encode_indices.c
    src/libs/decoders/internal/opus/silk/encode_pulses.c
    src/libs/decoders/internal/opus/silk/gain_quant.c
    src/libs/decoders/internal/opus/silk/init_decoder.c
    src/libs/decoders/internal/opus/silk/init_encoder.c
    src/libs/decoders/internal/opus/silk/inner_prod_aligned.c
    src/libs/decoders/internal/opus/silk/interpolate.c
    src/libs/decoders/internal/opus/silk/lin2log.c
    src/libs/decoders/internal/opus/silk/log2lin.c
    src/libs/decoders/internal/opus/silk/pitch_est_tables.c
    src/libs/decoders/internal/opus/silk/process_NLSFs.c
    src/libs/decoders/internal/opus/silk/quant_LTP_gains.c
    src/libs/decoders/internal/opus/silk/resampler.c
    src/libs/decoders/internal/opus/silk/resampler_down2.c
    src/libs/decoders/internal/opus/silk/resampler_down2_3.c
    src/libs/decoders/internal/opus/silk/resampler_private_AR2.c
    src/libs/decoders/internal/opus/silk/resampler_private_IIR_FIR.c
    src/libs/decoders/internal/opus/silk/resampler_private_down_FIR.c
    src/libs/decoders/internal/opus/silk/resampler_private_up2_HQ.c
    src/libs/decoders/internal/opus/silk/resampler_rom.c
    src/libs/decoders/internal/opus/silk/shell_coder.c
    src/libs/decoders/internal/opus/silk/sigm_Q15.c
    src/libs/decoders/internal/opus/silk/sort.c
    src/libs/decoders/internal/opus/silk/stereo_LR_to_MS.c
    src/libs/decoders/internal/opus/silk/stereo_MS_to_LR.c
    src/libs/decoders/internal/opus/silk/stereo_decode_pred.c
    src/libs/decoders/internal/opus/silk/stereo_encode_pred.c
    src/libs/decoders/internal/opus/silk/stereo_find_predictor.c
    src/libs/decoders/internal/opus/silk/stereo_quant_pred.c
    src/libs/decoders/internal/opus/silk/sum_sqr_shift.c
    src/libs/decoders/internal/opus/silk/table_LSF_cos.c
    src/libs/decoders/internal/opus/silk/tables_LTP.c
    src/libs/decoders/internal/opus/silk/tables_NLSF_CB_NB_MB.c
    src/libs/decoders/internal/opus/silk/tables_NLSF_CB_WB.c
    src/libs/decoders/internal/opus/silk/tables_gain.c
    src/libs/decoders/internal/opus/silk/tables_other.c
    src/libs/decoders/internal/opus/silk/tables_pitch_lag.c
    src/libs/decoders/internal/opus/silk/tables_pulses_per_block.c

    # libs/decoders/internal - opus/silk/float
    src/libs/decoders/internal/opus/silk/float/LPC_analysis_filter_FLP.c
    src/libs/decoders/internal/opus/silk/float/LPC_inv_pred_gain_FLP.c
    src/libs/decoders/internal/opus/silk/float/LTP_analysis_filter_FLP.c
    src/libs/decoders/internal/opus/silk/float/LTP_scale_ctrl_FLP.c
    src/libs/decoders/internal/opus/silk/float/apply_sine_window_FLP.c
    src/libs/decoders/internal/opus/silk/float/autocorrelation_FLP.c
    src/libs/decoders/internal/opus/silk/float/burg_modified_FLP.c
    src/libs/decoders/internal/opus/silk/float/bwexpander_FLP.c
    src/libs/decoders/internal/opus/silk/float/corrMatrix_FLP.c
    src/libs/decoders/internal/opus/silk/float/encode_frame_FLP.c
    src/libs/decoders/internal/opus/silk/float/energy_FLP.c
    src/libs/decoders/internal/opus/silk/float/find_LPC_FLP.c
    src/libs/decoders/internal/opus/silk/float/find_LTP_FLP.c
    src/libs/decoders/internal/opus/silk/float/find_pitch_lags_FLP.c
    src/libs/decoders/internal/opus/silk/float/find_pred_coefs_FLP.c
    src/libs/decoders/internal/opus/silk/float/inner_product_FLP.c
    src/libs/decoders/internal/opus/silk/float/k2a_FLP.c
    src/libs/decoders/internal/opus/silk/float/noise_shape_analysis_FLP.c
    src/libs/decoders/internal/opus/silk/float/pitch_analysis_core_FLP.c
    src/libs/decoders/internal/opus/silk/float/process_gains_FLP.c
    src/libs/decoders/internal/opus/silk/float/regularize_correlations_FLP.c
    src/libs/decoders/internal/opus/silk/float/residual_energy_FLP.c
    src/libs/decoders/internal/opus/silk/float/scale_copy_vector_FLP.c
    src/libs/decoders/internal/opus/silk/float/scale_vector_FLP.c
    src/libs/decoders/internal/opus/silk/float/schur_FLP.c
    src/libs/decoders/internal/opus/silk/float/sort_FLP.c
    src/libs/decoders/internal/opus/silk/float/warped_autocorrelation_FLP.c
    src/libs/decoders/internal/opus/silk/float/wrappers_FLP.c

    # libs/decoders/internal - opus/src
    src/libs/decoders/internal/opus/src/analysis.c
    src/libs/decoders/internal/opus/src/mapping_matrix.c
    src/libs/decoders/internal/opus/src/mlp.c
    src/libs/decoders/internal/opus/src/mlp_data.c
    src/libs/decoders/internal/opus/src/opus_decoder.c
    src/libs/decoders/internal/opus/src/opus_encoder.c
    src/libs/decoders/internal/opus/src/opus_multistream.c
    src/libs/decoders/internal/opus/src/opus_multistream_decoder.c
    src/libs/decoders/internal/opus/src/opus_multistream_encoder.c
    src/libs/decoders/internal/opus/src/opus_projection_decoder.c
    src/libs/decoders/internal/opus/src/opus_projection_encoder.c
    src/libs/decoders/internal/opus/src/opussrc.c
    src/libs/decoders/internal/opus/src/repacketizer.c

    # libs/decoders/internal - opusfile
    src/libs/decoders/internal/opusfile/http.c
    src/libs/decoders/internal/opusfile/info.c
    src/libs/decoders/internal/opusfile/internal.c
    src/libs/decoders/internal/opusfile/opusfile.c
    src/libs/decoders/internal/opusfile/stream.c
    src/libs/decoders/internal/opusfile/wincerts.c

    # libs/decoders/internal - speexdsp
    src/libs/decoders/internal/speexdsp/buffer.c
    src/libs/decoders/internal/speexdsp/fftwrap.c
    src/libs/decoders/internal/speexdsp/filterbank.c
    src/libs/decoders/internal/speexdsp/jitter.c
    src/libs/decoders/internal/speexdsp/kiss_fft_speex.c
    src/libs/decoders/internal/speexdsp/kiss_fftr.c
    src/libs/decoders/internal/speexdsp/mdf.c
    src/libs/decoders/internal/speexdsp/preprocess.c
    src/libs/decoders/internal/speexdsp/resample.c
    src/libs/decoders/internal/speexdsp/scal.c
    src/libs/decoders/internal/speexdsp/smallft.c

    # libs/fluidsynth
    src/libs/fluidsynth/fluid_adriver.c
    src/libs/fluidsynth/fluid_adsr_env.c
    src/libs/fluidsynth/fluid_aufile.c
    src/libs/fluidsynth/fluid_chan.c
    src/libs/fluidsynth/fluid_chorus.c
    src/libs/fluidsynth/fluid_cmd.c
    src/libs/fluidsynth/fluid_conv.c
    src/libs/fluidsynth/fluid_defsfont.c
    src/libs/fluidsynth/fluid_dll.c
    src/libs/fluidsynth/fluid_dsound.c
    src/libs/fluidsynth/fluid_event.c
    src/libs/fluidsynth/fluid_filerenderer.c
    src/libs/fluidsynth/fluid_gen.c
    src/libs/fluidsynth/fluid_hash.c
    src/libs/fluidsynth/fluid_iir_filter.c
    src/libs/fluidsynth/fluid_lfo.c
    src/libs/fluidsynth/fluid_list.c
    src/libs/fluidsynth/fluid_mdriver.c
    src/libs/fluidsynth/fluid_midi.c
    src/libs/fluidsynth/fluid_midi_router.c
    src/libs/fluidsynth/fluid_mod.c
    src/libs/fluidsynth/fluid_ramsfont.c
    src/libs/fluidsynth/fluid_rev.c
    src/libs/fluidsynth/fluid_ringbuffer.c
    src/libs/fluidsynth/fluid_rvoice.c
    src/libs/fluidsynth/fluid_rvoice_dsp.c
    src/libs/fluidsynth/fluid_rvoice_event.c
    src/libs/fluidsynth/fluid_rvoice_mixer.c
    src/libs/fluidsynth/fluid_sdl2.c
    src/libs/fluidsynth/fluid_seq.c
    src/libs/fluidsynth/fluid_seqbind.c
    src/libs/fluidsynth/fluid_settings.c
    src/libs/fluidsynth/fluid_synth.c
    src/libs/fluidsynth/fluid_sys.c
    src/libs/fluidsynth/fluid_tuning.c
    src/libs/fluidsynth/fluid_voice.c

    # libs/gui_tk
    src/libs/gui_tk/gui_tk.cpp

    # libs/passthroughio
    src/libs/passthroughio/passthroughio.cpp

    # libs/tinyfiledialogs
    src/libs/tinyfiledialogs/tinyfiledialogs.c

    # libs/zmbv
    src/libs/zmbv/zmbv.cpp

    # misc
    src/misc/clipboard.cpp
    src/misc/cross.cpp
    src/misc/ethernet.cpp
    src/misc/ethernet_nothing.cpp
    src/misc/ethernet_pcap.cpp
    src/misc/ethernet_slirp.cpp
    src/misc/iconvpp.cpp
    src/misc/messages.cpp
    src/misc/mkdir_p.cpp
    src/misc/programs.cpp
    src/misc/regionalloctracking.cpp
    src/misc/savestates.cpp
    src/misc/setup.cpp
    src/misc/shiftjis.cpp
    src/misc/support.cpp

    # output
    src/output/output_opengl.cpp
    src/output/output_surface.cpp
    src/output/output_tools.cpp
    src/output/output_tools_xbrz.cpp
    src/output/output_ttf.cpp

    # shell
    src/shell/shell.cpp
    src/shell/shell_batch.cpp
    src/shell/shell_cmds.cpp
    src/shell/shell_misc.cpp
)

# ── Windows-specific sources ─────────────────────────────────────────────────
if(IS_WINDOWS)
    list(APPEND DOSBOX_SOURCES
        src/debug/debug_win32.cpp
        src/dos/cdrom_aspi_win32.cpp
        src/dos/cdrom_ioctl_win32.cpp
        src/hardware/mic_input_win32.cpp
        src/hardware/RetroWaveLib/Platform/Win32_SerialPort.c
    )
endif()

# ── Direct3D sources (Windows only) ─────────────────────────────────────────
if(IS_WINDOWS AND ENABLE_D3D)
    list(APPEND DOSBOX_SOURCES
        src/output/direct3d/ScalingEffect.cpp
        src/output/direct3d/direct3d.cpp
        src/output/direct3d/hq2x_d3d.cpp
        src/output/output_direct3d.cpp
        src/output/output_direct3d11.cpp
    )
endif()

# ── MT32 sources ─────────────────────────────────────────────────────────────
if(ENABLE_MT32 AND CAN_MT32)
    list(APPEND DOSBOX_SOURCES
        src/libs/mt32/Analog.cpp
        src/libs/mt32/BReverbModel.cpp
        src/libs/mt32/Display.cpp
        src/libs/mt32/File.cpp
        src/libs/mt32/FileStream.cpp
        src/libs/mt32/LA32FloatWaveGenerator.cpp
        src/libs/mt32/LA32Ramp.cpp
        src/libs/mt32/LA32WaveGenerator.cpp
        src/libs/mt32/MidiStreamParser.cpp
        src/libs/mt32/Part.cpp
        src/libs/mt32/Partial.cpp
        src/libs/mt32/PartialManager.cpp
        src/libs/mt32/Poly.cpp
        src/libs/mt32/ROMInfo.cpp
        src/libs/mt32/SampleRateConverter.cpp
        src/libs/mt32/Synth.cpp
        src/libs/mt32/TVA.cpp
        src/libs/mt32/TVF.cpp
        src/libs/mt32/TVP.cpp
        src/libs/mt32/Tables.cpp
        src/libs/mt32/c_interface/c_interface.cpp
        src/libs/mt32/sha1/sha1.cpp
    )
endif()

# ── xBRZ sources ─────────────────────────────────────────────────────────────
if(ENABLE_XBRZ)
    list(APPEND DOSBOX_SOURCES
        src/libs/xBRZ/xbrz.cpp
    )
endif()

# ── Gamelink sources (SDL2 only) ─────────────────────────────────────────────
if(USE_SDL2 AND ENABLE_GAMELINK)
    list(APPEND DOSBOX_SOURCES
        src/gamelink/gamelink.cpp
        src/gamelink/gamelink_term.cpp
        src/output/output_gamelink.cpp
    )
endif()

# ── Create executable ───────────────────────────────────────────────────────
if(IS_WINDOWS)
    add_executable(dosbox-x WIN32 ${DOSBOX_SOURCES})
else()
    add_executable(dosbox-x ${DOSBOX_SOURCES})
endif()

# ── Include directories ─────────────────────────────────────────────────────
target_include_directories(dosbox-x PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/aviwriter
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/snd_pc98/cbus
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/snd_pc98/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/snd_pc98/generic
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/snd_pc98/sound
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/snd_pc98/sound/getsnd
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/snd_pc98/x11
)

if(ENABLE_MT32 AND CAN_MT32)
    target_include_directories(dosbox-x PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/libs/mt32
    )
endif()

# ── Compile definitions ─────────────────────────────────────────────────────
target_compile_definitions(dosbox-x PRIVATE
    _FILE_OFFSET_BITS=64
    PACKAGE="dosbox-x"
    PACKAGE_NAME="dosbox-x"
    PACKAGE_VERSION="${PROJECT_VERSION}"
    PACKAGE_STRING="dosbox-x ${PROJECT_VERSION}"
    PACKAGE_BUGREPORT="https://github.com/joncampbell123/dosbox-x/issues"
    PACKAGE_URL="https://dosbox-x.com"
    PACKAGE_TARNAME="dosbox-x"
    VERSION="${PROJECT_VERSION}"
)

if(IS_WINDOWS)
    target_compile_definitions(dosbox-x PRIVATE
        WIN32
        __WIN32__
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
    )
endif()

if(USE_SDL2)
    target_compile_definitions(dosbox-x PRIVATE C_SDL2=1)
else()
    target_compile_definitions(dosbox-x PRIVATE C_SDL1=1)
endif()

# CPU target
if(DOSBOX_CPU_TARGET STREQUAL "X86_64")
    target_compile_definitions(dosbox-x PRIVATE "C_TARGETCPU=X86_64")
elseif(DOSBOX_CPU_TARGET STREQUAL "X86")
    target_compile_definitions(dosbox-x PRIVATE "C_TARGETCPU=X86")
elseif(DOSBOX_CPU_TARGET STREQUAL "ARMV8LE")
    target_compile_definitions(dosbox-x PRIVATE "C_TARGETCPU=ARMV8LE")
elseif(DOSBOX_CPU_TARGET STREQUAL "ARMV7LE")
    target_compile_definitions(dosbox-x PRIVATE "C_TARGETCPU=ARMV7LE")
endif()

if(CAN_DYNAMIC_X86)
    target_compile_definitions(dosbox-x PRIVATE C_DYNAMIC_X86=1)
endif()

if(CAN_DYNREC)
    target_compile_definitions(dosbox-x PRIVATE C_DYNREC=1)
endif()

if(CAN_UNALIGNED)
    target_compile_definitions(dosbox-x PRIVATE C_UNALIGNED_MEMORY=1)
endif()

# Feature flags
if(ENABLE_FPU)
    target_compile_definitions(dosbox-x PRIVATE C_FPU=1)
    if(CAN_FPU_X86 AND NOT (CMAKE_SIZEOF_VOID_P EQUAL 8))
        target_compile_definitions(dosbox-x PRIVATE C_FPU_X86=1)
    endif()
endif()

if(ENABLE_DEBUG)
    target_compile_definitions(dosbox-x PRIVATE C_DEBUG=1 C_HEAVY_DEBUG=1)
endif()

if(ENABLE_OPENGL)
    target_compile_definitions(dosbox-x PRIVATE C_OPENGL=1)
endif()

if(IS_WINDOWS AND ENABLE_D3D)
    target_compile_definitions(dosbox-x PRIVATE C_DIRECT3D=1 C_D3DSHADERS=1 HAVE_D3D9_H=1)
endif()

if(ENABLE_MT32 AND CAN_MT32)
    target_compile_definitions(dosbox-x PRIVATE C_MT32=1)
endif()

if(ENABLE_XBRZ)
    target_compile_definitions(dosbox-x PRIVATE C_XBRZ=1 C_SURFACE_POSTRENDER_ASPECT=1 C_SURFACE_POSTRENDER_ASPECT_BATCH_SIZE=16)
endif()

if(ENABLE_PCAP)
    target_compile_definitions(dosbox-x PRIVATE C_PCAP=1)
endif()

if(ENABLE_PRINTER)
    target_compile_definitions(dosbox-x PRIVATE C_PRINTER=1)
endif()

if(USE_SDL2 AND ENABLE_GAMELINK)
    target_compile_definitions(dosbox-x PRIVATE C_GAMELINK=1)
endif()

target_compile_definitions(dosbox-x PRIVATE
    C_DIRECTSERIAL=1
    C_DIRECTLPT=1
    C_MODEM=1
    C_IPX=1
    C_SSHOT=1
    C_LIBPNG=1
    C_LIBZ=1
    C_FREETYPE=1
    C_SDL_NET=1
    ENVIRON_INCLUDED=1
    SIZEOF_UNSIGNED_CHAR=1
    SIZEOF_UNSIGNED_SHORT=2
    SIZEOF_UNSIGNED_INT=4
    SIZEOF_UNSIGNED_LONG=4
    SIZEOF_UNSIGNED_LONG_LONG=8
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_compile_definitions(dosbox-x PRIVATE SIZEOF_INT_P=8)
else()
    target_compile_definitions(dosbox-x PRIVATE SIZEOF_INT_P=4)
endif()

if(IS_WINDOWS)
    target_compile_definitions(dosbox-x PRIVATE HAVE_D3D9_H=1 HAVE_STDLIB_H=1 HAVE_STRING_H=1 HAVE_SYS_STAT_H=1)
    if(NOT USE_SDL2)
        target_compile_definitions(dosbox-x PRIVATE HAVE_DDRAW_H=1)
    endif()
endif()

# ── Find and link dependencies ──────────────────────────────────────────────

# On Windows, try the bundled VS libraries first, then fall back to find_package
if(IS_WINDOWS)
    set(VS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vs")

    if(USE_SDL2)
        if(EXISTS "${VS_DIR}/sdl2/include/SDL.h")
            target_include_directories(dosbox-x PRIVATE "${VS_DIR}/sdl2/include")
            message(STATUS "Using bundled SDL2 headers from vs/sdl2/include")
        else()
            find_package(SDL2 REQUIRED)
            target_include_directories(dosbox-x PRIVATE ${SDL2_INCLUDE_DIRS})
            target_link_libraries(dosbox-x PRIVATE ${SDL2_LIBRARIES})
        endif()
        if(EXISTS "${VS_DIR}/sdl2net")
            target_include_directories(dosbox-x PRIVATE "${VS_DIR}/sdl2net")
        endif()
    else()
        if(EXISTS "${VS_DIR}/sdl/include/SDL.h")
            target_include_directories(dosbox-x PRIVATE "${VS_DIR}/sdl/include")
            message(STATUS "Using bundled SDL1 headers from vs/sdl/include")
        else()
            find_package(SDL REQUIRED)
            target_include_directories(dosbox-x PRIVATE ${SDL_INCLUDE_DIRS})
            target_link_libraries(dosbox-x PRIVATE ${SDL_LIBRARIES})
        endif()
        if(EXISTS "${VS_DIR}/sdlnet")
            target_include_directories(dosbox-x PRIVATE "${VS_DIR}/sdlnet")
        endif()
    endif()

    if(EXISTS "${VS_DIR}/zlib")
        target_include_directories(dosbox-x PRIVATE "${VS_DIR}/zlib")
    endif()
    if(EXISTS "${VS_DIR}/libpng")
        target_include_directories(dosbox-x PRIVATE "${VS_DIR}/libpng")
    endif()
    if(EXISTS "${VS_DIR}/freetype/include")
        target_include_directories(dosbox-x PRIVATE "${VS_DIR}/freetype/include")
    endif()
    if(ENABLE_DEBUG AND EXISTS "${VS_DIR}/libpdcurses")
        target_include_directories(dosbox-x PRIVATE "${VS_DIR}/libpdcurses")
    endif()
    if(ENABLE_PCAP AND EXISTS "${VS_DIR}/pcap")
        target_include_directories(dosbox-x PRIVATE "${VS_DIR}/pcap")
    endif()

    # config.h from vs/ directory
    target_include_directories(dosbox-x PRIVATE "${VS_DIR}")

    # Windows system libraries
    target_link_libraries(dosbox-x PRIVATE
        winmm imm32 dsound dxguid SetupAPI version
        Iphlpapi Ws2_32 opengl32
    )
else()
    # Unix/macOS: use find_package / pkg-config
    include(FindPkgConfig)

    if(USE_SDL2)
        pkg_check_modules(SDL2 REQUIRED sdl2)
        target_include_directories(dosbox-x PRIVATE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(dosbox-x PRIVATE ${SDL2_LIBRARIES})

        pkg_check_modules(SDL2_NET sdl2_net)
        if(SDL2_NET_FOUND)
            target_include_directories(dosbox-x PRIVATE ${SDL2_NET_INCLUDE_DIRS})
            target_link_libraries(dosbox-x PRIVATE ${SDL2_NET_LIBRARIES})
        endif()
    else()
        pkg_check_modules(SDL REQUIRED sdl)
        target_include_directories(dosbox-x PRIVATE ${SDL_INCLUDE_DIRS})
        target_link_libraries(dosbox-x PRIVATE ${SDL_LIBRARIES})

        pkg_check_modules(SDL_NET SDL_net)
        if(SDL_NET_FOUND)
            target_include_directories(dosbox-x PRIVATE ${SDL_NET_INCLUDE_DIRS})
            target_link_libraries(dosbox-x PRIVATE ${SDL_NET_LIBRARIES})
        endif()
    endif()

    find_package(ZLIB REQUIRED)
    target_link_libraries(dosbox-x PRIVATE ZLIB::ZLIB)

    find_package(PNG REQUIRED)
    target_link_libraries(dosbox-x PRIVATE PNG::PNG)

    find_package(Freetype REQUIRED)
    target_link_libraries(dosbox-x PRIVATE Freetype::Freetype)

    if(ENABLE_DEBUG)
        find_package(Curses REQUIRED)
        target_include_directories(dosbox-x PRIVATE ${CURSES_INCLUDE_DIRS})
        target_link_libraries(dosbox-x PRIVATE ${CURSES_LIBRARIES})
    endif()

    if(ENABLE_OPENGL)
        find_package(OpenGL REQUIRED)
        target_link_libraries(dosbox-x PRIVATE OpenGL::GL)
    endif()

    if(ENABLE_PCAP)
        find_library(PCAP_LIBRARY pcap)
        if(PCAP_LIBRARY)
            target_link_libraries(dosbox-x PRIVATE ${PCAP_LIBRARY})
        else()
            message(WARNING "libpcap not found, disabling PCAP support")
            target_compile_definitions(dosbox-x PRIVATE -UC_PCAP)
        endif()
    endif()

    target_link_libraries(dosbox-x PRIVATE m)

    if(IS_LINUX)
        target_compile_definitions(dosbox-x PRIVATE
            _XOPEN_SOURCE=700
            _POSIX_C_SOURCE=200809L
            LINUX
            HAVE_UNISTD_H=1
            HAVE_NETINET_IN_H=1
            HAVE_SYS_SOCKET_H=1
            HAVE_PWD_H=1
        )
        target_link_libraries(dosbox-x PRIVATE pthread)

        find_library(ALSA_LIBRARY asound)
        if(ALSA_LIBRARY)
            target_compile_definitions(dosbox-x PRIVATE HAVE_ALSA=1)
            target_link_libraries(dosbox-x PRIVATE ${ALSA_LIBRARY})
        endif()
    endif()

    if(IS_MACOS)
        target_compile_definitions(dosbox-x PRIVATE MACOSX)
        find_library(CARBON_FRAMEWORK Carbon)
        find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
        find_library(COREMIDI_FRAMEWORK CoreMIDI)
        find_library(AUDIOUNIT_FRAMEWORK AudioUnit)
        find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
        target_link_libraries(dosbox-x PRIVATE
            ${CARBON_FRAMEWORK}
            ${COREFOUNDATION_FRAMEWORK}
            ${COREMIDI_FRAMEWORK}
            ${AUDIOUNIT_FRAMEWORK}
            ${AUDIOTOOLBOX_FRAMEWORK}
        )
    endif()
endif()

# ── Compiler flags ──────────────────────────────────────────────────────────
if(MSVC)
    target_compile_options(dosbox-x PRIVATE /W3 /MP)
    target_compile_definitions(dosbox-x PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
    )
else()
    target_compile_options(dosbox-x PRIVATE
        -Wall
        -Wno-sign-compare
        -Wno-unused-variable
        -Wno-unused-parameter
        -Wno-missing-field-initializers
        -Wno-strict-aliasing
        -Wno-format-zero-length
    )
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(dosbox-x PRIVATE
            -Wno-address-of-packed-member
        )
    endif()
endif()

# ── Windows resource file ───────────────────────────────────────────────────
if(IS_WINDOWS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/winres.rc")
    target_sources(dosbox-x PRIVATE src/winres.rc)
endif()

# ── Install ─────────────────────────────────────────────────────────────────
install(TARGETS dosbox-x RUNTIME DESTINATION bin)
