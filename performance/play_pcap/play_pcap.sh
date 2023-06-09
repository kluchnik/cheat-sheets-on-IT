#!/bin/bash
# Generation of network packets by pcap files
HELP='
-------------------------------------------\n
Generation of network packets by pcap files\n
-------------------------------------------\n
Usage: play_pcap.sh [start|stop] [OPTIONS]\n
-------------------------------------------\n
OPTIONS start:\n
--help			\t\t\t - Information sheet\n
--iface=XX		\t\t - Network interface name\n
--mode=[onece|endless|loop] \t - Generation mode [once|endless|loop]\n
--loop=X		\t\t\t - Number of loops, only --mode=loop\n
--unique-ip		\t\t\t - Modify IP addresses each loop iteration\n
			\t\t\t\t to generate unique flows. Only --mode=loop\n
--speed=[def|top|mbps]	\t - Network Packet Rate\n
			\t\t\t\t def - as in pcap file\n
			\t\t\t\t top - max speed\n
			\t\t\t\t mbps - set speed\n
--mbps=XX		\t\t\t - Set speed in mbps, only --speed=mbps\n
--option="XXXX"		\t\t - Additional options tcpreplay\n
--preload-pcap		\t\t - Preloads packets into RAM before sending\n
--pcap="./pcap/*.pcap"	\t - Path to folder for pcap files\n
--pid="/tmp/gnppcap.pid" \t - Directory to save the pid\n
\n
OPTIONS stop:\n
--pid="/tmp/gnppcap.pid" \t - Directory to save the pid\n
------------------------------------------\n
EXIT CODE:\n
0 - status ok\n
1 - OPTIONS error\n
2 - tcpreplay not install\n
3 - permission denied tcpreplay\n
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
    --iface=*)
      IFACE=${1#--iface=}
      shift
      ;;
    --mode=*)
      MODE=${1#--mode=}
      shift
      ;;
    --loop=*)
      lOOP=${1#--loop=}
      shift
      ;;
    --unique-ip)
      UNIQUE_IP='--unique-ip'
      shift
      ;;
    --speed=*)
      SPEED=${1#--speed=}
      shift
      ;;
    --mbps=*)
      MBPS=${1#--mbps=}
      shift
      ;;
    --option=*)
      OPTION=${1#--option=}
      shift
      ;;
    --preload-pcap)
      PRELOAD_PCAP='--preload-pcap'
      shift
      ;;
    --pcap=*)
      PCAP_PATH=${1#--pcap=}
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

if [[ ${STATUS} == 'start' ]]; then
  #----------------
  # check tcpreplay
  #---------------
  GEN_PATH=$(whereis tcpreplay | awk '{ print $2 }')
  if [[ -z ${GEN_PATH} ]]; then
    echo "> Traffic generation utility tcpreplay not found"
    exit 2
  fi
  LS_GEN=$(ls -l $GEN_PATH)
  if [[ -z $(echo $LS_GEN | grep $(whoami)) && -z $(echo $LS_GEN | grep -E '^...s') ]]; then
    echo "> Permission denied:"
    echo $LS_GEN
    echo "> Run as available user or change file permissions:"
    echo "> sudo chmod u+s ${GEN_PATH}"
    exit 3
  fi
  #---------------
  # check interface
  #---------------
  INTERFACES=$(tcpreplay --listnics)
  if [[ -z ${IFACE} ]]; then
    echo "> Interface not set: ${IFACE}"
    echo "> Use the option: --iface=<name interface>"
    echo ${INTERFACES}
    exit 1
  fi
  if [[ -z $(echo ${INTERFACES} | grep -E " ${IFACE} ") ]]; then
    echo "> Interface not found: ${IFACE}"
    echo ${INTERFACES}
    exit 1
  fi
  #---------------
  # check mode
  #---------------
  if [[ -z ${MODE} ]]; then
    MODE='onece'
  fi
  if [[ -z $(echo ${MODE} | grep -E "onece|endless|loop") ]]; then
    echo "> Error mode: ${MODE}";
    echo "> Use the option: --mode=[onece|endless|loop]"
    exit 1
  fi
  if [[ ${MODE} == 'loop' ]]; then
    if [[ -z ${LOOP} ]]; then
      LOOP=1
    fi
  fi
  #---------------
  # check unique ip
  #---------------
  if [[ ${UNIQUE_IP} == '--unique-ip' ]]; then
    UNIQUE_OPT='--unique-ip --unique-ip-loops=1'
  fi
  #---------------
  # check speed
  #---------------
  if [[ -z ${SPEED} ]]; then
    SPEED='def'
  fi
  if [[ -z $(echo ${SPEED} | grep -E "def|top|mbps") ]]; then
    echo "> Error speed: ${SPEED}";
    echo "> Use the option: --speed=[def|top|mbps]"
    exit 1
  fi
  if [[ ${SPEED} == 'def' ]]; then
    SPEED_OPT=''
  elif [[ ${SPEED} == 'top' ]]; then
    SPEED_OPT='--topspeed'
  elif [[ ${SPEED} == 'mbps' ]]; then
    if [[ -z ${MBPS} ]]; then
      MBPS=1
    fi
    SPEED_OPT="--mbps=${MBPS}"
  fi
  #---------------
  # check pcap path
  #---------------
  if [[ -z ${PCAP_PATH} ]]; then
    PCAP_PATH='./pcap/*.pcap'
  fi
fi
#---------------
# check pid file
#---------------
if [[ -z ${PID_FILE} ]]; then
  PID_FILE='/tmp/gnppcap.pid'
fi

#-----------------------------
# Run Generation
#-----------------------------

function start_gen_npkg() {
  echo "Run tcpreplay mode: ${MODE}"
  if [[ ${MODE} == 'onece' ]]; then
     echo "Run: tcpreplay -i ${IFACE} ${PRELOAD_PCAP} ${SPEED_OPT} --no-flow-stats ${OPTIONS} ${PCAP_PATH}"
     tcpreplay -i ${IFACE} ${PRELOAD_PCAP} ${SPEED_OPT} --no-flow-stats ${OPTIONS} ${PCAP_PATH} &
  elif [[ ${MODE} == 'endless' ]]; then
    echo "Run: tcpreplay -i ${IFACE} ${PRELOAD_PCAP} ${SPEED_OPT} --no-flow-stats ${OPTIONS} ${PCAP_PATH}"
    while true; do
      tcpreplay -i ${IFACE} ${PRELOAD_PCAP} ${SPEED_OPT} --no-flow-stats ${OPTIONS} ${PCAP_PATH}
    done &
  elif [[ ${MODE} == 'endless' ]]; then
    echo "Run: tcpreplay -i ${IFACE} --loop=${LOOP} ${UNIQUE_OPT} ${PRELOAD_PCAP} \
${SPEED_OPT} --no-flow-stats ${OPTIONS} ${PCAP_PATH}"
    tcpreplay -i ${IFACE} --loop=${LOOP} ${UNIQUE_OPT} ${PRELOAD_PCAP} \
${SPEED_OPT} --no-flow-stats ${OPTIONS} ${PCAP_PATH} $
  fi
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
echo "Run generation of network packets by pcap files"
echo "-----------------------------"
echo "OPTIONS:"
echo -e "\tstatus = ${STATUS}"
if [[ ${STATUS} == 'start' ]]; then
  echo -e "\tinterface = ${IFACE}"
  echo -e "\tmode = ${MODE}"
  if [[ ${MODE} == loop ]]; then
    echo -e "\tloop = ${LOOP}"
    echo -e "\tunique options = ${UNIQUE_OPT}"
  fi
  echo -e "\tmode speed = ${SPEED}"
  echo -e "\tspeed options = ${SPEED_OPT}"
  echo -e "\tother options = ${OPTIONS}"
  echo -e "\tpreload pcap = ${PRELOAD_PCAP}"
  echo -e "\tpcap path = ${PCAP_PATH}"
  echo -e "\tpid file = ${PID_FILE}"
  echo "-----------------------------"
  start_gen_npkg
elif [[ ${STATUS} == 'stop' ]]; then
  echo -e "\tpid file = ${PID_FILE}"
  echo "-----------------------------"
  stop_gen_npkg
fi

exit 0
