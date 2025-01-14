/****************************************************************************
 * apps/examples/nxscope/nxscope_main.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include "logging/nxscope/nxscope.h"

/****************************************************************************
 * Private Type Definition
 ****************************************************************************/

struct nxscope_thr_env_s
{
  FAR struct nxscope_s *nxs;
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nxscope_cb_userid
 ****************************************************************************/

int nxscope_cb_userid(FAR void *priv, uint8_t id, FAR uint8_t *buff)
{
  UNUSED(priv);

  printf("--> nxscope_cb_userid: id=%d\n", id);

  return OK;
}

/****************************************************************************
 * Name: nxscope_cb_start
 ****************************************************************************/

int nxscope_cb_start(FAR void *priv, bool start)
{
  UNUSED(priv);

  printf("--> nxscope_cb_start: start=%d\n", start);

  return OK;
}

/****************************************************************************
 * Name: nxscope_samples_thr
 ****************************************************************************/

static FAR void *nxscope_samples_thr(FAR void *arg)
{
  FAR struct nxscope_thr_env_s *envp = arg;
  FAR uint8_t                  *ptr  = NULL;
  uint32_t                      i    = 0;
  float                         v[3];

  DEBUGASSERT(envp);

  printf("nxscope_samples_thr\n");

  /* Initialize float vector */

  v[0] = -1.0f;
  v[1] = 0.0f;
  v[2] = 1.0f;

  while (1)
    {
      /* Channel 0 */

      nxscope_put_uint8(envp->nxs, 0, i);

      /* Channel 1 */

      nxscope_put_int8(envp->nxs, 1, -1);

      /* Channel 2 */

      nxscope_put_uint16(envp->nxs, 2, 300);

      /* Channel 3 */

      nxscope_put_int16(envp->nxs, 3, -300);

      /* Channel 4 */

      nxscope_put_uint32(envp->nxs, 4, 35000);

      /* Channel 5 */

      nxscope_put_int32(envp->nxs, 5, -35000);

      /* Channel 6 */

      nxscope_put_uint64(envp->nxs, 6, 4294967296);

      /* Channel 7 */

      nxscope_put_int64(envp->nxs, 7, -4294967296);

      /* Channel 8 */

      nxscope_put_float(envp->nxs, 8, 1.0f);

      /* Channel 9 */

      nxscope_put_double(envp->nxs, 9, 1.11111111);

      /* Channel 10 */

      nxscope_put_ub8(envp->nxs, 10, ftob8(1.0f));

      /* Channel 11 */

      nxscope_put_b8(envp->nxs, 11, ftob8(-1.0f));

      /* Channel 12 */

      nxscope_put_ub16(envp->nxs, 12, ftob16(1.0f));

      /* Channel 13 */

      nxscope_put_b16(envp->nxs, 13, ftob16(-1.0f));

#ifdef CONFIG_HAVE_LONG_LONG
      /* Channel 14 */

      nxscope_put_ub32(envp->nxs, 14, dtob32(1.0));

      /* Channel 15 */

      nxscope_put_b32(envp->nxs, 15, dtob32(-1.0));
#endif

      /* Channel 16 */

      nxscope_put_vfloat(envp->nxs, 16, v, 3);

      /* Channel 17 */

      ptr = (FAR uint8_t *) &i;
      nxscope_put_vfloat_m(envp->nxs, 17, v, 3, ptr, sizeof(uint32_t));

      /* Channel 18 */

      nxscope_put_none_m(envp->nxs, 18, ptr, sizeof(uint32_t));

      i += 1;

      usleep(100);
    }

  return NULL;
}

/****************************************************************************
 * Name: nxscope_charlog_thr
 ****************************************************************************/

static FAR void *nxscope_charlog_thr(FAR void *arg)
{
  FAR struct nxscope_thr_env_s *envp = arg;
  int                           i    = 0;

  DEBUGASSERT(envp);

  printf("nxscope_charlog_thr\n");

  while (1)
    {
      /* Channel 19 - send hello with metadata */

      nxscope_put_vchar_m(envp->nxs, 19, "hello", 64,
                          (FAR uint8_t *)&i, sizeof(int));

      i += 1;

      usleep(100000);
    }

  return NULL;
}

#ifdef CONFIG_LOGGING_NXSCOPE_CRICHANNELS
/****************************************************************************
 * Name: nxscope_crichan_thr
 ****************************************************************************/

static FAR void *nxscope_crichan_thr(FAR void *arg)
{
  FAR struct nxscope_thr_env_s *envp = arg;
  uint8_t                       i    = 0;

  DEBUGASSERT(envp);

  printf("nxscope_crichan_thr\n");

  while (1)
    {
      /* Channel 20 */

      nxscope_put_uint8(envp->nxs, 20, i);

      i += 1;

      usleep(100000);
    }

  return NULL;
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nxscope_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  struct nxscope_s            nxs;
  int                         ret = OK;
  pthread_t                   thread;
  struct nxscope_thr_env_s    env;
  struct nxscope_cfg_s        nxs_cfg;
  union nxscope_chinfo_type_u u;
  struct nxscope_intf_s       intf;
  struct nxscope_proto_s      proto;
  struct nxscope_callbacks_s  cbs;
#ifdef CONFIG_LOGGING_NXSCOPE_INTF_SERIAL
  struct nxscope_ser_cfg_s    nxs_ser_cfg;
#endif
#ifdef CONFIG_LOGGING_NXSCOPE_INTF_DUMMY
  struct nxscope_dummy_cfg_s  nxs_dummy_cfg;
#endif

  /* Default serial protocol */

  ret = nxscope_proto_ser_init(&proto, NULL);
  if (ret < 0)
    {
      printf("ERROR: nxscope_proto_ser_init failed %d\n", ret);
      goto errout_noproto;
    }

#ifdef CONFIG_LOGGING_NXSCOPE_INTF_SERIAL
  /* Configuration */

  nxs_ser_cfg.path     = CONFIG_EXAMPLES_NXSCOPE_SERIAL_PATH;
  nxs_ser_cfg.nonblock = true;
  nxs_ser_cfg.baud     = CONFIG_EXAMPLES_NXSCOPE_SERIAL_BAUD;

  /* Initialize serial interface */

  ret = nxscope_ser_init(&intf, &nxs_ser_cfg);
  if (ret < 0)
    {
      printf("ERROR: nxscope_ser_init failed %d\n", ret);
      goto errout_nointf;
    }
#endif

#ifdef CONFIG_LOGGING_NXSCOPE_INTF_DUMMY
  /* Configuration */

  nxs_dummy_cfg.res = 0;

  /* Initialize dummy interface */

  ret = nxscope_dummy_init(&intf, &nxs_dummy_cfg);
  if (ret < 0)
    {
      printf("ERROR: nxscope_dummy_init failed %d\n", ret);
      goto errout_nointf;
    }
#endif

  /* Connect callbacks */

  cbs.userid_priv = NULL;
  cbs.userid = nxscope_cb_userid;

  cbs.start_priv = NULL;
  cbs.start = nxscope_cb_start;

  /* Initialize nxscope */

  nxs_cfg.intf_cmd      = &intf;
  nxs_cfg.intf_stream   = &intf;
  nxs_cfg.proto_cmd     = &proto;
  nxs_cfg.proto_stream  = &proto;
  nxs_cfg.callbacks     = &cbs;
  nxs_cfg.channels      = 32;
  nxs_cfg.streambuf_len = CONFIG_EXAMPLES_NXSCOPE_STREAMBUF_LEN;
  nxs_cfg.rxbuf_len     = CONFIG_EXAMPLES_NXSCOPE_RXBUF_LEN;
#ifdef CONFIG_LOGGING_NXSCOPE_CRICHANNELS
  nxs_cfg.cribuf_len    = CONFIG_EXAMPLES_NXSCOPE_CRIBUF_LEN;
#endif
  nxs_cfg.rx_padding    = CONFIG_EXAMPLES_NXSCOPE_RX_PADDING;

  ret = nxscope_init(&nxs, &nxs_cfg);
  if (ret < 0)
    {
      printf("ERROR: nxscope_init failed %d\n", ret);
      goto errout_nonxscope;
    }

  /* Create channels */

  /* Point data channels */

  u.s.dtype = NXSCOPE_TYPE_UINT8;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 0, "chan0", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_INT8;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 1, "chan1", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_UINT16;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 2, "chan2", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_INT16;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 3, "chan3", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_UINT32;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 4, "chan4", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_INT32;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 5, "chan5", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_UINT64;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 6, "chan6", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_INT64;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 7, "chan7", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_FLOAT;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 8, "chan8", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_DOUBLE;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 9, "chan9", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_UB8;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 10, "chan10", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_B8;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 11, "chan11", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_UB16;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 12, "chan12", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_B16;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 13, "chan13", u.u8, 1, 0);

#ifdef CONFIG_HAVE_LONG_LONG
  u.s.dtype = NXSCOPE_TYPE_UB32;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 14, "chan14", u.u8, 1, 0);

  u.s.dtype = NXSCOPE_TYPE_B32;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 15, "chan15", u.u8, 1, 0);
#endif

  /* Vector data channel */

  u.s.dtype = NXSCOPE_TYPE_FLOAT;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 16, "chan16", u.u8, 3, 0);

  /* Vector data channel with metadata */

  u.s.dtype = NXSCOPE_TYPE_FLOAT;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 17, "chan17", u.u8, 3, 4);

  /* No-data channel with metadata */

  u.s.dtype = NXSCOPE_TYPE_NONE;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 18, "chan18", u.u8, 0, 4);

  /* Char channel with metadata */

  u.s.dtype = NXSCOPE_TYPE_CHAR;
  u.s._res  = 0;
  u.s.cri   = 0;
  nxscope_chan_init(&nxs, 19, "chan19", u.u8, 64, 4);

#ifdef CONFIG_LOGGING_NXSCOPE_CRICHANNELS
  /* Critical channel */

  u.s.dtype = NXSCOPE_TYPE_UINT8;
  u.s._res  = 0;
  u.s.cri   = 1;
  nxscope_chan_init(&nxs, 20, "chan20c", u.u8, 1, 0);
#endif

  /* Channels 20-31: reserved for future use */

  /* Create samples thread */

  env.nxs = &nxs;
  ret = pthread_create(&thread, NULL, nxscope_samples_thr, &env);
  if (ret != OK)
    {
      printf("ERROR: pthread_create failed %d\n", ret);
      goto errout;
    }

  /* Create char log thread */

  env.nxs = &nxs;
  ret = pthread_create(&thread, NULL, nxscope_charlog_thr, &env);
  if (ret != OK)
    {
      printf("ERROR: pthread_create failed %d\n", ret);
      goto errout;
    }

#ifdef CONFIG_LOGGING_NXSCOPE_CRICHANNELS
  /* Create critical channel thread */

  env.nxs = &nxs;
  ret = pthread_create(&thread, NULL, nxscope_crichan_thr, &env);
  if (ret != OK)
    {
      printf("ERROR: pthread_create failed %d\n", ret);
      goto errout;
    }
#endif

#ifdef CONFIG_EXAMPLES_NXSCOPE_FORCE_ENABLE
  /* Enable channels and enable stream */

  nxscope_chan_all_en(&nxs, true);
  nxscope_stream_start(&nxs, true);
#endif

  /* Main loop */

  while (1)
    {
      /* Flush stream data */

      ret = nxscope_stream(&nxs);
      if (ret < 0)
        {
          printf("ERROR: nxscope_stream failed %d\n", ret);
        }

      /* Handle recv data */

      ret = nxscope_recv(&nxs);
      if (ret < 0)
        {
          printf("ERROR: nxscope_recv failed %d\n", ret);
        }

      usleep(100000);
    }

errout:

  /* Deinit nxscope */

  nxscope_deinit(&nxs);

errout_nonxscope:

  /* Deinit interface */

#if defined(CONFIG_LOGGING_NXSCOPE_INTF_SERIAL)
  nxscope_ser_deinit(&intf);
#endif
#if defined(CONFIG_LOGGING_NXSCOPE_INTF_DUMMY)
  nxscope_dummy_deinit(&intf);
#endif

errout_nointf:

  /* Deinit protocol */

#if defined(CONFIG_LOGGING_NXSCOPE_PROTO_SER)
  nxscope_proto_ser_deinit(&proto);
#endif

errout_noproto:

  return 0;
}
