FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        g++ make openmpi-bin libopenmpi-dev openssh-server openssh-client \
        netpbm time python3 ca-certificates \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m -s /bin/bash mpi \
    && mkdir -p /run/sshd /home/mpi/.ssh \
    && ssh-keygen -A \
    && ssh-keygen -t ed25519 -N "" -f /home/mpi/.ssh/id_ed25519 \
    && cp /home/mpi/.ssh/id_ed25519.pub /home/mpi/.ssh/authorized_keys \
    && printf "Host *\n  StrictHostKeyChecking no\n  UserKnownHostsFile /dev/null\n  LogLevel ERROR\n" > /home/mpi/.ssh/config \
    && chown -R mpi:mpi /home/mpi/.ssh \
    && chmod 700 /home/mpi/.ssh \
    && chmod 600 /home/mpi/.ssh/id_ed25519 /home/mpi/.ssh/authorized_keys /home/mpi/.ssh/config

WORKDIR /work
CMD ["/usr/sbin/sshd", "-D"]
