import { Platform } from 'react-native';

export const ARCHERY_MULTICAST_HOST = '224.25.26.27';
export const ARCHERY_UDP_PORT = 4665;

export function buildHomeIdleArcheryPacket(): Uint8Array {
    const buf = new Uint8Array(15);
    buf[0] = 0x54; // 'T'
    buf[1] = 0x53; // 'S'
    for (let i = 2; i <= 12; i++) buf[i] = 0x30; // '0'
    buf[13] = 0x30;
    buf[14] = 0x38; // '8'
    return buf;
}

export type SendResult = { ok: true } | { ok: false; message: string };

export async function sendArcheryMulticast(payload: Uint8Array): Promise<SendResult> {
    if (Platform.OS === 'web') {
        return { ok: false, message: 'UDP is not available on web'};
    }
    try {
        const dgram = (await import('react-native-udp')).default;
        const socket = dgram.createSocket({ type: 'udp4' });
    await new Promise<void>((resolve, reject) => {
      const fail = (err: Error) => {
        socket.close();
        reject(err);
      };
      socket.once('error', fail);
      socket.bind(0, () => {
        socket.send(
          payload,
          0,
          payload.length,
          ARCHERY_UDP_PORT,
          ARCHERY_MULTICAST_HOST,
          (err?: Error) => {
            socket.removeListener('error', fail);
            socket.close();
            if (err) reject(err);
            else resolve();
          }
        );
      });
    });
    return { ok: true };
    }
}

export async function broadcastHomeIdleToMatrix(): Promise<SendResult> {
    return sendArcheryMulticast(buildHomeIdleArcheryPacket());
}