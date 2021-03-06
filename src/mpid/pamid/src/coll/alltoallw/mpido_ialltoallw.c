/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/*  (C)Copyright IBM Corp.  2007, 2011  */
/**
 * \file src/coll/alltoallv/mpido_alltoallw.c
 * \brief ???
 */
/*#define TRACE_ON*/

#include <mpidimpl.h>

int MPIDO_Ialltoallw(const void *sendbuf,
                    const int *sendcounts,
                    const int *senddispls,
                    const MPI_Datatype *sendtypes,
                    void *recvbuf,
                    const int *recvcounts,
                    const int *recvdispls,
                    const MPI_Datatype *recvtypes,
                    MPID_Comm *comm_ptr,
                    MPID_Request **request)
{
   if(comm_ptr->rank == 0)
      TRACE_ERR("Entering MPIDO_Ialltoallw\n");

   /*if (unlikely((data_size == 0) || (user_selected_type == MPID_COLL_USE_MPICH)))*/
   {
      /*
       * If the mpich mpir non-blocking collectives are enabled, return without
       * first constructing the MPID_Request. This signals to the
       * MPIR_Ialltoallw_impl() function to invoke the mpich nbc implementation
       * of MPI_Ialltoallw().
       */
      if (MPIDI_Process.mpir_nbc != 0)
       return 0;

      /*
       * MPIR_* nbc implementation is not enabled. Fake a non-blocking
       * MPIR_Ialltoallw() with a blocking MPIR_Alltoallw().
       */
      if(unlikely(MPIDI_Process.verbose >= MPIDI_VERBOSE_DETAILS_ALL && comm_ptr->rank == 0))
         fprintf(stderr,"Using MPICH blocking alltoallw algorithm\n");

      int mpierrno = 0;
      int rc = MPIR_Alltoallw_impl(sendbuf, sendcounts, senddispls, sendtypes,
                                  recvbuf, recvcounts, recvdispls, recvtypes,
                                  comm_ptr, &mpierrno);

      /*
       * The blocking alltoallw has completed - create and complete a
       * MPID_Request object so the MPIR_Ialltoallw_impl() function does not
       * perform an additional ialltoallw.
       */
      MPID_Request * mpid_request = MPID_Request_create_inline();
      mpid_request->kind = MPID_COLL_REQUEST;
      *request = mpid_request;
      MPIDI_Request_complete_norelease_inline(mpid_request);

      return rc;
   }


   TRACE_ERR("Leaving ialltoallw\n");
   return 0;
}
