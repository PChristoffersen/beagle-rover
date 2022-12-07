import { DefaultColors } from "../Colors";


export default function Motor() {
    const color = DefaultColors.motorCaseColor;
    const dotColor = DefaultColors.dotColor;
    const strokeColor = DefaultColors.motorStrokeColor;

    return (
        /*
        <svg
            width="100mm"
            height="100mm"
            viewBox="0 0 100 100"
            xmlns="http://www.w3.org/2000/svg"
        >
        */
        <g transform="translate(-50.0, -53.2)">
            <path
                style={{
                    fill: dotColor,
                    stroke: "none",
                }}
                d="M48 36h4v19h-4z"
            />
            <path
                style={{
                    fill: dotColor,
                    stroke: "none",
                }}
                d="M30 47.5h40v4H30z"
            />
            <path
                style={{
                    fill: color,
                    stroke: strokeColor,
                    strokeWidth: 0.5,
                }}
                d="M48.872 85.243c-.03-.031-.056-1.06-.056-2.286v-2.23h-.505c-.615 0-.68-.085-.68-.883 0-.432-.028-.556-.125-.556-.068 0-.276-.162-.463-.36-.34-.36-.34-.36-.342-1.057l-.002-.697-2.265-.023-2.264-.022-.023-1.757-.023-1.757h-5.206l-1.036-1.038-1.036-1.039V56.817l-2.706-2.718c-2.457-2.467-2.732-2.772-2.985-3.306-.25-.53-.278-.664-.278-1.35 0-.683.027-.819.268-1.312.147-.303.437-.722.643-.932.206-.21 1.408-1.2 2.671-2.201l2.297-1.82.002-5.242.002-5.243 1.076-1.086 1.075-1.087h9.364v-.247c0-.488.042-.515.803-.515h.7l.05-.254c.04-.202.095-.254.265-.254h.213l.022-7.26.021-7.26 1.857-.023c1.429-.018 1.872.002 1.925.084.037.06.068 3.451.07 7.537v7.43h.71c.773 0 .814.026.814.515v.247h9.341l1.087 1.075 1.087 1.076.002 5.253.002 5.254 2.297 1.82c1.263 1.001 2.465 1.992 2.671 2.201.206.21.496.629.643.932.24.493.268.629.268 1.312 0 .686-.028.82-.278 1.35-.253.534-.528.839-2.985 3.306l-2.706 2.718v14.732l-1.098 1.087-1.098 1.086-2.54-.038-2.542-.038-.023 1.741-.022 1.742-2.265.022-2.265.023-.002.697c-.002.696-.003.698-.342 1.057-.187.198-.395.36-.463.36-.097 0-.124.124-.124.556 0 .785-.071.883-.638.883h-.463v2.122c0 1.301-.034 2.186-.088 2.286-.08.15-.186.164-1.17.164-.597 0-1.11-.025-1.14-.056zm1.775-30.67c.282-.13.378-.234.498-.55.243-.635.06-1.422-.384-1.66-.355-.19-.965-.198-1.37-.019-.323.143-.409.233-.534.56-.176.463-.126 1.08.115 1.407.304.413 1.075.533 1.675.261zm-17.96-4.062c.759-.426.707-1.813-.082-2.2-.356-.174-.985-.156-1.322.04-.68.391-.786 1.556-.191 2.085.36.32 1.095.356 1.595.075zm36.068 0c.759-.426.707-1.813-.082-2.2-.356-.174-.985-.156-1.322.04-.68.391-.786 1.556-.191 2.085.36.32 1.095.356 1.595.075zM50.72 38.488c.759-.426.707-1.813-.082-2.2-.356-.174-.985-.155-1.322.04-.68.392-.786 1.556-.191 2.085.36.321 1.095.356 1.595.075z"
            />
        </g>
        /*
        </svg>
        */
    )
}