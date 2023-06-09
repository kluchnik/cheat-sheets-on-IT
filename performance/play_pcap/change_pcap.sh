#!/bin/bash
# Changing parameters in the pickup file
HELP='
------------------------------------------\n
Changing parameters in the pickup file\n
------------------------------------------\n
Usage: change_pcap.sh [OPTIONS]\n
------------------------------------------\n
OPTIONS:\n
--help			\t\t - Information sheet\n
--ip_src=X.X.X.X	\t\t - IP address of source\n
--ip_dst=X.X.X.X	\t\t - IP address of destination\n
--in_path="./pcap/*.pcap"	\t - Directory to load pcap files\n
--out_path="./npcap/*.pcap"	\t - Directory to save pcap files\n
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
    --ip_src=*)
      IP_SRC=${1#--ip_src=}
      shift
      ;;
    --ip_dst=*)
      IP_DST=${1#--ip_dst=}
      shift
      ;;
    --in_path=*)
      IN_PCAP_PATH=${1#--in_path=}
      shift
      ;;
    --out_path=*)
      OUT_PCAP_PATH=${1#--out_path=}
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
if [[ -z ${IP_SRC} ]]; then
  echo "> Not set IP source: ${IP_SEC}"
  echo "> Set option: --ip_src=<ip source>"
  exit 1
fi
if [[ -z ${IP_DST} ]]; then
  echo "> Not set IP destination: ${IP_DST}"
  echo "> Set option: --ip_dst=<ip destination>"
  exit 1
fi
if [[ -z ${IN_PCAP_PATH} ]]; then
  echo "> Not set input directory for pcap: ${IN_PCAP_PATH}"
  echo "> Set option: --in_pcap=<path input directory for pcap>"
  exit 1
fi
if [[ -z ${OUT_PCAP_PATH} ]]; then
  echo "> Not set output directory for pcap: ${OUT_PCAP_PATH}"
  echo "> Set option: --out_pcap=<path output directory for pcap>"
  exit 1
fi

#-----------------------------
# Run change
#-----------------------------

function get_in_pcap() {
  echo $(ls ${IN_PCAP_PATH})
}

function change_pcap() {
  PCAP_IN=$1
  PCAP_NAME=$(basename "${PCAP_IN}")
  PCAP_OUT=$(echo ${OUT_PCAP_PATH}/${PCAP_NAME})
  echo "Change: ${PCAP_IN} -> ${PCAP_OUT}"
  tcprewrite --dstipmap=0.0.0.0/0:${IP_DST} --srcipmap=0.0.0.0/0:${IP_SRC} \
--infile=${PCAP_IN} --outfile=${PCAP_OUT} --fixcsum
}

echo "-----------------------------"
echo "Run Changing parameters in the pickup file"
echo "-----------------------------"
echo "OPTIONS:"
echo -e "\tip source = ${IP_SRC}"
echo -e "\tip destination = ${IP_DST}"
echo -e "\tinput pcap directory = ${IN_PCAP_PATH}"
echo -e "\toutput pcap directory = ${OUT_PCAP_PATH}"
echo "-----------------------------"
LIST_PCAP=$(get_in_pcap)
for ITEM in ${LIST_PCAP}; do
  change_pcap ${ITEM}
done

exit 0
fre
