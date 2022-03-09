import { DefaultColors } from "../Colors";


export default function Connector() {
    const joinColor = DefaultColors.joinColor;
    const strokeColor = DefaultColors.strokeColor;

    return (
        /*
        <svg
            width="70mm"
            height="8mm"
            viewBox="0 0 70 8"
            xmlns="http://www.w3.org/2000/svg"
        >
        */
        <g transform="translate(4,-4) rotate(90)">
            <path
                style={{
                    fill: joinColor,
                    stroke: strokeColor,
                    strokeWidth: 0.5,
                }}
                d="M.049 3.276C.284 1.95 1.352.718 2.697.222c.972-.358 1.234-.34 7.253.486 7.54 1.036 7.799 1.06 10.33 1.003 1.63-.037 3.131-.152 7.026-.537C32.21.69 32.287.685 35.011.685s2.802.005 7.705.49c3.895.384 5.396.499 7.027.536 2.53.058 2.788.033 10.33-1.003 6.021-.827 6.24-.842 7.254-.484 1.126.398 2.172 1.469 2.528 2.59.209.653.209 1.763 0 2.417-.356 1.121-1.402 2.192-2.528 2.59-1.013.357-1.233.343-7.255-.485-7.54-1.035-7.799-1.06-10.329-1.003-1.631.038-3.132.152-7.027.537-4.903.484-4.981.49-7.705.49s-2.802-.006-7.705-.49c-3.895-.385-5.396-.5-7.027-.537-2.53-.057-2.788-.032-10.33 1.003-6.021.828-6.24.842-7.254.484C1.585 7.428.514 6.342.185 5.274c-.178-.58-.236-1.43-.136-1.998zM2.51 4.539c.134.4.616.88 1 .996.486.145.775.117 1.253-.12.355-.178.475-.298.652-.652.346-.695.26-1.298-.266-1.85-.503-.53-1.168-.63-1.866-.283a1.569 1.569 0 0 0-.773 1.91zm61.976 0c.134.4.616.88 1 .996.486.145.775.117 1.253-.12.355-.178.475-.298.652-.652.346-.695.26-1.298-.266-1.85-.503-.53-1.168-.63-1.866-.283a1.569 1.569 0 0 0-.773 1.909z"
            />
        </g>
        /*
        </svg>
        */
    )
}