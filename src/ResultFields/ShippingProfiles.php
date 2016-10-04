<?php

namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class ShippingProfiles
 * @package ElasticExport\ResultFields
 */
class ShippingProfiles extends ResultFields
{
    /*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

    /**
     * Geizhals constructor.
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ArrayHelper $arrayHelper)
    {
		$this->arrayHelper = $arrayHelper;
    }

    /**
     * Generate result fields.
     * @param  array<FormatSetting> $formatSettings = []
     * @return array
     */
    public function generateResultFields(array $formatSettings = []):array
    {
        $settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

        return [
            'itemBase'=> [
                'id',
            ],
            'itemShippingProfilesList' => [
                'id',
                'name',
                'backendName',
                'tags'
            ],
        ];
    }
}
