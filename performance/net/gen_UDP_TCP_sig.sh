#!/bin/bash
# Generation of network packets by signature
HELP='
------------------------------------------\n
Generation of network packets by signature\n
------------------------------------------\n
Usage: gen_npkg_sig.sh [start|stop] [OPTIONS]\n
------------------------------------------\n
OPTIONS start:\n
--help			\t\t - Information sheet\n
--proto=[udp|tcp]	\t - Protocol selection, only udp or tcp\n
--ip=X.X.X.X		\t\t - IP address of the remote host to which\n
			\t\t\t the network packet will be sent\n
--port=XXXX		\t\t - Port number to which the network packet\n
			\t\t\t will be sent\n
--sig="XXXXX"		\t\t - Signature included in network packet\n
--timeout=X		\t\t - Timeout, sec\n
--flow=X		\t\t - flow of network packets in one cycle\n
--pid="/tmp/gnps.pid"	\t - Directory to save the pid\n
\n
OPTIONS stop:\n
--pid="/tmp/gnps.pid"	\t - Directory to save the pid\n
------------------------------------------\n
EXIT CODE:\n
0 - status ok\n
1 - OPTIONS error
'

#-----------------------------
# Parsing args
#-----------------------------
while [[ $# -gt 0 ]]; do
  case $1 in
    --help)
      echo -e ${HELP}
      exit 1
      ;;
    start)
      STATUS=${1}
      shift
      ;;
    stop)
      STATUS=${1}
      shift
      ;;
    --proto=*)
      PROTO=${1#--proto=}
      shift
      ;;
    --ip=*)
      IP=${1#--ip=}
      shift
      ;;
    --port=*)
      PORT=${1#--port=}
      shift
      ;;
    --sig=*)
      SIG=${1#--sig=}
      shift
      ;;
    --timeout=*)
      TIMEOUT=${1#--timeout=}
      shift
      ;;
    --flow=*)
      FLOW=${1#--flow=}
      let FLOW-=1
      shift
      ;;
    --pid=*)
      PID_FILE=${1#--pid=}
      shift
      ;;
    *)
      echo -e $HELP
      exit 1
      ;;
  esac
done

#-----------------------------
# Validation args
#-----------------------------
if [[ -z ${STATUS} ]]; then
  echo "> error status = ${STATUS}";
  echo "> status must be start or stop";
  echo "> run script with key: --help";
  exit 1
fi
if [[ -z ${PROTO} ]]; then
  PROTO='udp'
fi
if [[ -z ${IP} ]]; then
  IP='127.0.0.1'
fi
if [[ -z ${PORT} ]]; then
  PORT='2000'
fi
if [[ -z ${SIG} ]]; then
  SIG='test msg'
fi
if [[ -z ${TIMEOUT} ]]; then
  TIMEOUT='1'
fi
if [[ -z ${FLOW} ]]; then
  FLOW='0'
fi
if [[ -z ${PID_FILE} ]]; then
  PID_FILE='/tmp/gnps.pid'
fi

#-----------------------------
# Run Generation
#-----------------------------

function start_gen_npkg() {
  while true; do
    NEW_PORT=${PORT}
    for FP in $(seq 0 ${FLOW}); do
      let NEW_PORT+=FP
      echo ${SIG} > /dev/${PROTO}/${IP}/${NEW_PORT}
    done
    sleep ${TIMEOUT}
  done &
  PID=$!
  echo ${PID} > ${PID_FILE}
  echo "Start generation network pkg on pid: ${PID}"
}

function stop_gen_npkg() {
  PID=$(cat ${PID_FILE})
  echo "Stop generation network pkg on pid: ${PID}"
  kill -TERM -- ${PID}
}

echo "-----------------------------"
echo "Run generation of network packets by signature"
echo "-----------------------------"
echo "OPTIONS:"
echo -e "\tstatus = ${STATUS}"
if [[ ${STATUS} == 'start' ]]; then
  echo -e "\tprotocol = ${PROTO}"
  echo -e "\tip = ${IP}"
  echo -e "\tstart port = ${PORT}"
  let END_PORT=PORT+FLOW
  echo -e "\tend port = ${END_PORT}"
  echo -e "\tsignature = ${SIG}"
  echo -e "\ttimeout = ${TIMEOUT}"
  echo -e "\tpid file = ${PID_FILE}"
  echo "-----------------------------"
  start_gen_npkg
elif [[ ${STATUS} == 'stop' ]]; then
  echo -e "\tpid file = ${PID_FILE}"
  echo "-----------------------------"
  stop_gen_npkg
fi

exit 0
