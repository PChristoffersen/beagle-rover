import { DefaultColors } from "../Colors";


export default function ServoArm() {
    const joinColor = DefaultColors.joinColor;
    const strokeColor = DefaultColors.strokeColor;
    const dotColor = DefaultColors.dotColor;

    return (
        /*
        <svg
            width="42mm"
            height="18mm"
            viewBox="0 0 42 18"
            xmlns="http://www.w3.org/2000/svg"
        >
        */
        <g transform="translate(-21, -5.2)">
            <circle
                style={{
                fill: dotColor,
                stroke: "none",
                }}
                cx={39}
                cy={15}
                r={2}
            />
            <circle
                style={{
                    fill: dotColor,
                    stroke: "none",
                }}
                cx={3}
                cy={15}
                r={2}
            />
            <path
                style={{
                    fill: joinColor,
                    stroke: strokeColor,
                    strokeWidth: 0.5,
                }}
                d="M2.003 17.893c-.634-.235-.971-.463-1.325-.895-.806-.983-.956-2.093-.602-4.452.212-1.407.83-2.953 1.718-4.3.849-1.284 2.774-2.843 4.297-3.479C7.258 4.28 19.856.107 20.46.007a3.17 3.17 0 0 1 1.149 0c.604.1 13.202 4.273 14.37 4.76 1.522.636 3.447 2.195 4.296 3.48 1.327 2.01 1.884 3.956 1.825 6.38-.027 1.137-.04 1.207-.304 1.751-.369.757-.926 1.244-1.719 1.504-1.544.507-3.094-.188-3.845-1.725-.202-.413-.231-.595-.293-1.841-.074-1.482-.116-1.66-.551-2.316-.65-.979-1.7-1.661-2.797-1.816-.802-.114-22.312-.114-23.114 0-1.11.157-2.146.837-2.817 1.847-.394.593-.527 1.133-.527 2.132 0 1.487-.219 2.145-.986 2.966-.54.576-1.185.864-2.02.9-.519.023-.781-.008-1.124-.136zm1.614-1.563c.743-.316 1.024-1.286.607-2.093-.258-.5-1.203-.74-1.84-.468-.743.316-1.024 1.286-.607 2.093.259.5 1.201.74 1.84.468zm36.068 0c.743-.316 1.024-1.286.607-2.093-.258-.5-1.203-.74-1.84-.468-.743.316-1.024 1.286-.607 2.093.259.5 1.201.74 1.84.468zM21.695 5.964c.548-.354.628-1.262.152-1.738-.837-.836-2.213-.04-1.9 1.099.108.39.32.613.706.746.37.127.738.09 1.042-.107z"
            />
        </g>
        /*
        </svg>
        */
    )
}